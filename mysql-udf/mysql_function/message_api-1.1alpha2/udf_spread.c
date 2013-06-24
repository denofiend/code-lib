/* Copyright (C) 2006 MySQL AB

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 

This product uses software developed by Spread Concepts LLC for use in the 
Spread toolkit. For more information about Spread see http://www.spread.org
*/

#include "udf_spread.h"
#include "api_options.h"


/* This UDF file implements a reliable messaging API for MySQL.

    INT send_mesg(CHAR group,CHAR message, [CHAR group_member])
      send <message> to <group>.
      Returns TRUE if message was sent successfully.
      If <group_member> is included, also check whether <group_member> matches
      a prefix of some member of the group; if not, return FALSE.  
      On error, return NULL.
    INT join_mesg_group(CHAR group) 
       Join <group> and listen for messages.  This function returns a positive
       integer <handle> that must be used in later calls to recv_mesg().  
    CHAR recv_mesg(INT handle)
       Blocks listening for messages. Returns a message, or NULL as an exception
    INT leave_mesg_group(int handle) 
      Leave group and disconnect.
    INT track_memberships(CHAR group)
      Track membership of a group.  Returns a handle that can be disconnected 
      using leave_mesg_group().
    INT mesg_handle(CHAR query) 
      Implements a simple query syntax, and returns the pre-existing message
      handle requested in the query.
    CHAR mesg_status([INT handle])
      Returns some status information on a message handle, or on all handles.

    The functions rely on reliable group communications implemented by the 
    Spread Toolkit.
    
    This product uses software developed by Spread Concepts LLC for use in the 
    Spread toolkit. For more information about Spread see http://www.spread.org
*/


static struct group_table     private_names;
static struct group_table     tracked_groups;
static pthread_once_t         init_group_tables_once = PTHREAD_ONCE_INIT;

static struct spread_context  spread_pool[POOL_SIZE];
static struct outbox          outboxes[SEND_POOL_SIZE];
static pthread_once_t         init_outboxes_once    = PTHREAD_ONCE_INIT;
static int                    send_pool_is_initialized = 0;
static pthread_mutex_t        send_pool_mutex       = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t        recv_pool_mutex       = PTHREAD_MUTEX_INITIALIZER;

static int16                  base_tag;


/* Spread "send" calls share a small pool of connections.  In theory, when we 
   link with the thread-safe Spread library, all of the Spread send calls could 
   share a a single connection.  In practice we create a small connection pool 
   in order to reduce mutex contention inside of Spread. 
   (Specificially, around line 1021 in SP_internal_multicast in sp.c, Spread
   holds a mutex on a mailbox around the actual send() call, and we want to
   reduce the contention for that.)
   
   The connections in the send pool are shared randomly and without any locking. 
   
   The only use of send_pool_mutex is to protect the original SP_connect calls
   used to open the connections.
*/


int initialize_sender(int slot) {
  register int err ;

  spread_pool[slot].status = SPREAD_CTX_CONNECTING;
  
  /*  int  SP_connect( const char *spread_name, const char *private_name, 
                       int priority, int group_membership, mailbox *mbox, 
                       char *private_group)
  */
  err= SP_connect(SPREAD_NAME, NULL, 0, 0, & spread_pool[slot].mbox, 
                  spread_pool[slot].name.private );
  if(err == ACCEPT_SESSION) {
    spread_pool[slot].status = SPREAD_CTX_CONNECTED;
    spread_pool[slot].outbox = & outboxes[slot];
    group_table_op(OP_CREATE, & private_names, slot, spread_pool[slot].name.private);
  }
  else spread_pool[slot].status = SPREAD_CTX_DISCONNECTED;
  return err;
}


void initialize_outbox(int slot) {
  int i;
  
  pthread_mutex_init (& outboxes[slot].lock, NULL);
  pthread_cond_init(& outboxes[slot].telephone, NULL);
  outboxes[slot].n_broadcast = 0;
  outboxes[slot].max_used_tag = 0;
  for(i=0 ; i < OUT_BOX_SIZE ; i++) {
    outboxes[slot].mesg[i].mthread_slot = 0;
    outboxes[slot].mesg[i].status = BOX_CLEAR;
    outboxes[slot].mesg[i].sequence = 0;
  }
}

void initialize_outboxes() {
  int i;
  
  for(i = 0 ; i < SEND_POOL_SIZE ; i++)
    initialize_outbox(i);
}


int initialize_send_pool() {
  int err, i;
  int success = 0;
  int return_val = ACCEPT_SESSION;
  time_t timespec;

  pthread_once(& init_outboxes_once, initialize_outboxes);
  pthread_once(& init_group_tables_once, initialize_group_tables);

  pthread_mutex_lock(&send_pool_mutex);
    if(! send_pool_is_initialized) {
      for(i=0 ; i < SEND_POOL_SIZE; i++) {
        err = initialize_sender(i);
        if(err == ACCEPT_SESSION) success++; 
        else return_val = err;
      }
    }
    if(success) {
      time(&timespec);
      seed_rand((int) timespec);
      base_tag = get_rand() % 24000;
      send_pool_is_initialized = 1;
    }
  pthread_mutex_unlock(&send_pool_mutex);
  return return_val;
}


int get_recv_pool_connection(char *private_name) {
  int slot;
  int error = 0;
  int rv;

  
  /* Get a free context from the receive pool.
     Hold the mutex for just long enough to find a slot and set 
     its status to "not free" 
  */
  pthread_mutex_lock(&recv_pool_mutex);
  for(slot= RECV_POOL ; (spread_pool[slot]).status != SPREAD_CTX_FREE 
      && slot < POOL_SIZE ;
      slot++); 
  if(slot < POOL_SIZE) 
    spread_pool[slot].status = SPREAD_CTX_CONNECTING;
  else 
    error = 1;
  pthread_mutex_unlock(&recv_pool_mutex);
  
  if(! error) {
    /*  int  SP_connect( const char *spread_name, const char *private_name, 
                         int priority, int group_membership, mailbox *mbox, 
                         char *private_group)
    */
    rv = SP_connect(SPREAD_NAME, private_name, 0, 1, 
                    &(spread_pool[slot].mbox), 
                    spread_pool[slot].name.private);
    if(rv == ACCEPT_SESSION) {
      spread_pool[slot].status = SPREAD_CTX_CONNECTED;
    }
    else {
      spread_pool[slot].status = SPREAD_CTX_FREE;  /* correct? */
      SP_error(rv); /* writes to the MySQL error log */
      error = 1;
    }
  }
  return error ? -1 : slot ;
}  


static char * spread_diagnostic(int e) {
  switch(e) {
    case COULD_NOT_CONNECT:
      return "Could not connect to the Spread daemon (" SPREAD_NAME 
      "). Is it running?";
    case REJECT_VERSION:
      return "Spread connection rejected. Version mismatch.";
    case REJECT_NOT_UNIQUE:
      return "Spread connection rejected. Name not unique.";
    case ILLEGAL_GROUP:
      return "Spread error.  Illegal group name.";
    default:
      SP_error(e);
      return "Spread error.  Please check the MySQL error log.";
  }
}


/* Smart handling of spread disconnects. */
/* things to look out for here:
   Even if you reconnect the whole send pool, there are still recv connections.
   Don't create a loop or race condition where, after the spread daemon is
   bounced, you disconnect all of your newly established connections each time
   an old invalid one is discovered.
*/
/* TO DO: remove the slot from the private_names hash table? */

void handle_spread_disconnect(int err, int slot) {
  SP_error(err);
  pthread_mutex_lock(&send_pool_mutex);
    if(send_pool_is_initialized) 
      send_pool_is_initialized = 0;
  pthread_mutex_unlock(&send_pool_mutex);
}  

void handle_illegal_session(int err, int slot) {
    fprintf(stderr, 
      "- Message API: Illegal session error on message handle %d (mbox %d).\n"
      "- This could be caused by a receiver that was not able to keep up with the \n"
      "- message flow; if so, consider increasing MAX_SESSION_MESSAGES in Spread.\n",
      slot, spread_pool[slot].mbox);
}

my_bool bad_recv_slot(int slot) {
  if(slot < RECV_POOL || slot > POOL_SIZE) return 1;
  if(spread_pool[slot].status < SPREAD_CTX_JOINED) return 1;
  return 0;
}



/* SQL send_mesg() returns:
  1 if message was sent,
  0 if message was sent but recipient is not joined,
*/

my_bool send_mesg_init(UDF_INIT *initid, UDF_ARGS *args, char *err_msg)
{
  int err;
  int slot;
    
  initid->maybe_null = 0; 
  if(! send_pool_is_initialized) {
    err = initialize_send_pool();
    if(err != ACCEPT_SESSION) {
      strncpy(err_msg,spread_diagnostic(err),MYSQL_ERRMSG_SIZE);
      return 1;
    }
  }

  if((args->arg_count < 2) || (args->arg_count > 3 )) {
    strncpy(err_msg,"send_mesg(): wrong number of arguments",MYSQL_ERRMSG_SIZE);
    return 1;
  }
  args->arg_type[0] = STRING_RESULT;
  args->arg_type[1] = STRING_RESULT;

  if(args->arg_count == 2) {
    /* For non-reliable sends, choose a slot here. 
       We want a way to distribute connections somewhat evenly in the connection
       pool.  And we want to do this with serializing behind any resource, or 
       going into the kernel, or wasting CPU.
       Solution: take the address of "args" and shift it 5 bits to the right
    */
    slot = (((unsigned int) args) >> 5) % SEND_POOL_SIZE ;
    initid->ptr = (char *)  & (spread_pool[slot]);
    if(spread_pool[slot].status < SPREAD_CTX_CONNECTED) 
    {
      strncpy(err_msg,"Not connected to Spread daemon.", MYSQL_ERRMSG_SIZE);
      return 1;
    }
  }
  else {
    /* Sending a guaranteed message */
    initid->ptr = NULL;  /* send_guaranteed_mesg() will choose a slot */
    
    args->arg_type[2] = STRING_RESULT; 

    if(   ( args->args[0] )     /* check for untracked static groupname */
       && ( group_table_op(OP_LOOKUP,& tracked_groups,0,args->args[0]) 
            == GROUP_NOT_FOUND)) 
        {
          snprintf(err_msg,MYSQL_ERRMSG_SIZE,
              "You must call track_memberships(\"%s\") before "
              "sending a guaranteed message to a group member.",args->args[0]);
          return 1;
        }
  }
  
  return 0;
}


long long send_mesg(UDF_INIT *initid, UDF_ARGS *args, 
                    char *is_null, char *error)
{
  long long val = 0;

  if(args->args[0] == NULL)
    *error = 1;
  else if(args->arg_count == 2) 
     val = send_spread_message(initid, args, is_null, error);
  else if(args->args[2]) 
     val = send_guaranteed_message(initid, args, is_null, error);
  else *error = 1;
  
  return val;
}



long long send_spread_message(UDF_INIT *initid, UDF_ARGS *args, 
                     char *is_null, char *error)
{
  int err;
  struct spread_context *ctx;
  int16 mess_type = 0;
  
  ctx = (struct spread_context *) initid->ptr;
  
  /* int  SP_multicast(  mailbox  mbox,  service  service_type,  
    const  char *group, int16 mess_type, 
    int mess_len,  const char *mess);  
  */
  err = SP_multicast(ctx->mbox, FIFO_MESS, args->args[0], mess_type, 
                     (int) args->lengths[1], args->args[1]);

  if ((err > 0) || (err == 0 && args->lengths[1] == 0))
  {
    ctx->n.sent++;
    return (long long) 1;
  }
  
  if (err == CONNECTION_CLOSED) {
    ctx->status = SPREAD_CTX_DISCONNECTED;
    handle_spread_disconnect(err,-1);
  }

  *error = 1;
  return (long long) 0; 
}
  
  
long long send_guaranteed_message(UDF_INIT *initid, UDF_ARGS *args, 
                       char *is_null, char *error)
{
  struct spread_context *ctx;
  struct membership *m;
  int16 mess_type = 0;
  int members_slot, tag, err;
  int is_locked = 0;
  unsigned long mesg_seq_number;
  static struct timespec tm = { 0 , 250000000 } ; /* 250 ms. */
  int slot;

  /* args[0] is the group name, args[1] is the message.
     args[2] names the required recipient. */
  
  slot = get_rand() % SEND_POOL_SIZE;
  ctx = & spread_pool[slot];

  members_slot = group_table_op(OP_LOOKUP,& tracked_groups,0,args->args[0]);
  if((spread_pool[members_slot].status != SPREAD_CTX_TRACK_MEMBERSHIPS) 
     || ( (m = spread_pool[members_slot].m) == NULL) ) {
        *error = 1;
        return (long long) 0;
  } 
  pthread_mutex_lock(& ctx->outbox->lock);
    for(tag = 0; (ctx->outbox->mesg[tag].status != BOX_CLEAR) && (tag < OUT_BOX_SIZE); 
        tag++);
    if(tag == OUT_BOX_SIZE) {
      /* no free tags! */
      udf_assert(0);
      pthread_mutex_unlock(& ctx->outbox->lock);
      *error = 1;
      return (long long) 0;
    }
    ctx->outbox->mesg[tag].status = BOX_SENDING;
    ctx->outbox->mesg[tag].mthread_slot = members_slot;
    if(tag > ctx->outbox->max_used_tag) ctx->outbox->max_used_tag = tag;
  pthread_mutex_unlock(& ctx->outbox->lock);
  mess_type = base_tag + tag ;

  /* int  SP_multicast(  mailbox  mbox,  service  service_type,  
                        const  char *group, int16 mess_type, 
                        int mess_len,  const char *mess);  
  */
  err = SP_multicast(ctx->mbox, SAFE_MESS, args->args[0], mess_type, 
                     (int) args->lengths[1], args->args[1]);
  if (err > -1 ) { /* success */
    ctx->n.sent++;
    
    while(1) {
      mesg_seq_number = delivery_broker(BOX_SENT,ctx->outbox,tag,is_locked);
      if(mesg_seq_number) break;
      /* pthread_cond_timedwait() returns 0 if you have the lock, or ETIMEDOUT */
      err = pthread_cond_timedwait(& ctx->outbox->telephone, & ctx->outbox->lock, & tm);
      if(err == ETIMEDOUT) ctx->outbox->mesg[tag].flags.timed_out = 1;
      is_locked = err ? 0 : 1;
    }
    ctx->outbox->mesg[tag].flags.timed_out = 0;

    return is_a_member(m, mesg_seq_number, args->args[2]);
  } 

  /* We reach this point only if SP_multicast returned an error */
    
  SP_error(err);
  if (err == CONNECTION_CLOSED) {
    ctx->status = SPREAD_CTX_DISCONNECTED;
    handle_spread_disconnect(err,-1);
  }
  
  ctx->outbox->mesg[tag].status = BOX_CLEAR;
  
  *error = 1;
  return (long long) 0; 
}


sequence_number delivery_broker(outbox_tag op, struct outbox *outbox, int16 tag, 
                                sequence_number seq) {
  register sequence_number return_value = 0;
  my_bool sender_needs_alert = 0;
  int i, err = 0;

  /* If send_guaranteed_message() calls with seq set to 1, 
     then we already hold the lock */
  if(! ((op == BOX_SENT) && (seq == 1)))
    err = pthread_mutex_lock(& outbox->lock) ;
  udf_assert(! err);
  if(op == BOX_SENT) {
    /* Called by the sender */
    switch(outbox->mesg[tag].status) {
      case BOX_SENDING:
        /* sender has arrived here first */
        outbox->mesg[tag].status = BOX_SENT;
        break;
      case BOX_SENT:
        /* Spurious wakeup from pthread_cond_broadcast() meant for someone else */
        break ;
      case BOX_RECVD:
      case BOX_NOT_RECVD:
        /* mthread was here first */
        outbox->mesg[tag].status = BOX_CLEAR;
        outbox->mesg[tag].mthread_slot = 0;
        return_value = outbox->mesg[tag].sequence;
        break;
      default: /* impossible! */
        udf_assert(0) ;
    }
  }
  else if(op == BOX_RECVD) {
    /* called by the memberships thread */

    /* If the tag is set to BOX_SENT, then the sender has already been
       here, and will need to be notified that the message has now arrived. */
    sender_needs_alert =  (outbox->mesg[tag].status == BOX_SENT);

    outbox->mesg[tag].sequence = seq;
    outbox->mesg[tag].status = BOX_RECVD;
    return_value = 0;
  }
  else if(op == BOX_CLOSED) {
    /* called by the memberships thread; handle all remaining unverified messages;
       "tag" is set to the mthread_slot of the dying mthread */
    
    for(i = 0 ; i < OUT_BOX_SIZE ; i++) {
      if(   (outbox->mesg[i].mthread_slot == tag)
          && ((outbox->mesg[i].status == BOX_SENDING) || (outbox->mesg[i].status == BOX_SENT))
        )
      {
        if(outbox->mesg[i].status == BOX_SENT) sender_needs_alert = 1;
        outbox->mesg[tag].sequence = seq;   /* !?!?!?! is this a typo? */
        outbox->mesg[i].status = BOX_NOT_RECVD;
      }
    }
  }
  else {
    /* It should not be possible to reach this point */
    udf_assert(0);
  }
    
  err = pthread_mutex_unlock(& outbox->lock);
  udf_assert(! err) ;
  
  if(sender_needs_alert) {
    err = pthread_cond_broadcast(& outbox->telephone);
    udf_assert(! err);
    outbox->n_broadcast++;
  }  
  return return_value;
}


/*
 The receive calls work this way:
 join_mesg_group() maps to SP_connect() and SP_join().  
 The handler that is returned is an index into the recv_pool array.
 The handler must be passed to recv_message(), and it must be closed
 with leave_mesg_group().
 leave_mesg_group() maps to SP_leave(), but the socket is not disconnected
 until you call recv_mesg() up through your final message until it returns
 NULL.
*/


my_bool join_mesg_group_init(UDF_INIT *initid, UDF_ARGS *args, char *err_msg)
{
  initid->maybe_null = 0;

  if((args->arg_count < 1) || (args->arg_count > 2)) {
    strncpy(err_msg, "join_mesg_group(): wrong number of arguments",MYSQL_ERRMSG_SIZE);
    return 1;
  }

  args->arg_type[0] = STRING_RESULT;
  if(args->arg_count == 2) 
    args->arg_type[1] = STRING_RESULT;
  return 0;
}


long long join_mesg_group(UDF_INIT *initid, UDF_ARGS *args, 
                          char *is_null, char *error)
{
  register int err = 0;
  int slot;
  char *private_name;
  
  if(args->args[0] == NULL) {
    *error = 1 ;
    return 0;
  }
  
  if(args->arg_count > 1) 
    private_name = args->args[1];
  else 
    private_name = NULL;

  slot = get_recv_pool_connection(private_name);
  if (slot < 0) *error = 1;
      
  if(! *error) {
    /* Join a group */
    /* int SP_join( mailbox mbox, const char *group); */
    err = SP_join(spread_pool[slot].mbox, args->args[0]);
    if(err < 0)
      *error = 1;
  }
  
  if(! *error) {
    spread_pool[slot].status = SPREAD_CTX_JOINED;
    strncpy(spread_pool[slot].name.group, args->args[0], MAX_GROUP_NAME);
    group_table_op(OP_CREATE, & private_names, slot, spread_pool[slot].name.private);
  }
    
  return (long long) slot;
}  


static inline void disconnect_recv_slot(int slot) {
  pthread_mutex_lock(&recv_pool_mutex);
  SP_disconnect(spread_pool[slot].mbox);
  spread_pool[slot].status = SPREAD_CTX_FREE;
  spread_pool[slot].mbox = 0;
  spread_pool[slot].n.recv = 0;
  pthread_mutex_unlock(&recv_pool_mutex);
}


int ascii_to_timeval(char *s, int len, struct timeval *tv) {
  char *p, *q;
  char tmp[16];
  int lsz, rsz, extra;
  int j, t;
  
  rsz = lsz = 0;
  tv->tv_sec = tv->tv_usec = 0;
  p = s;
  
  if( *p != '.') {
    /* full seconds, left of the decimal */
    while(isdigit(*p) && (lsz < 16) && (lsz < len)) p++, lsz++;
    if((lsz > 15) || (lsz < 1)) return 0;
    strcpy(tmp, "");
    strncat(tmp,s,lsz);
    tv->tv_sec = atoi(tmp);
    if(lsz == len) return 1;
  }
  /* next character must be the decimal point */
  if(*p != '.') return 0;
  extra = 1; /* length of the decimal point */
  q = ++p;
  /* string from q to p is fractional seconds right of the decimal */
  while(isdigit(*q) && rsz < 7) q++, rsz++;
  while(isdigit(*q)) q++, extra++;
  if( rsz + lsz + extra != len) return 0;
  
  strcpy(tmp, "");
  strncat(tmp,p,rsz);
  j = atoi(tmp);  
  for( t = 1 ; rsz++ < 6 ; t *= 10);
  tv->tv_usec = j * t;
  return 1;
}


/* SQL recv_mesg takes a message handle and an (optional) options string.
   It usually returns a message string.
   It can return NULL in three cases:
   It received a self-leave message, the supplied handle was NULL, 
   or a timeout was supplied, and it the call timed out before a message
   was available.
*/
my_bool recv_mesg_init(UDF_INIT *initid, UDF_ARGS *args, char *err_msg)
{
  long long slot = 0;
  opt_parser_return r;
  struct message_function_init *f;

  initid->maybe_null = 1;
  
  if((args->arg_count < 1) || (args->arg_count > 2)){
    strncpy(err_msg, "recv_mesg(): wrong number of arguments",MYSQL_ERRMSG_SIZE);
    return 1;
  }
  args->arg_type[0] = INT_RESULT;

  if(args->args[0]) {
    /* The handle is given as a constant */
    slot = *((long long *) args->args[0]);
    if(bad_recv_slot(slot)) {
      strncpy(err_msg ,"Invalid message handle.",
              MYSQL_ERRMSG_SIZE);
      return 1;
    }
    if(spread_pool[slot].status == SPREAD_CTX_TRACK_MEMBERSHIPS) {
      strncpy(err_msg,"You cannot receive messages on a message handle that is "
       "also being used to track memberships.",MYSQL_ERRMSG_SIZE);
      return 1;
    }
  }
  if(args->arg_count == 2) { /* Options */
    args->arg_type[1] = STRING_RESULT;
    f = calloc(1,sizeof(struct message_function_init));
    f->options = malloc(sizeof(all_api_options));
    initialize_options(f->options);
    initid->ptr = (char *) f; 
    if(args->args[1]) {
      /* Options are given as a constant */ 
      if(slot) {
        if(spread_pool[slot].mbox > FD_SETSIZE) {
        /* This is a very strange error, but MySQL could use a lot of open file
          descriptors before Spread opens a connection, so I think it is possible 
        */
          strncpy(err_msg,"recv_mesg(): cannot use timeout, FD_SETSIZE is too small", 
                  MYSQL_ERRMSG_SIZE);      
          return 1;
        }
      }  
      r = parse_api_options(f->options, OPF_timeout, & (f->set_options), args->args[1]);
      if(r) {
        OPTIONS_ERROR("recv_mesg",r);
        return 1;
      }
      if(f->set_options & OPF_timeout) {
        if(! ascii_to_timeval(f->options[OP_timeout].value,
                              f->options[OP_timeout].value_len, & (f->timeval))) 
        {
          strncpy(err_msg,"recv_mesg(): invalid timeout", MYSQL_ERRMSG_SIZE);      
          return 1;
        }
      }
    }
  }
    
  return 0;
}


char * recv_mesg(UDF_INIT *initid, UDF_ARGS *args, char *result,
                  unsigned long *length, char *is_null, char *error)
{
  int rcv;
  service svctype = 0;
  char sender[MAX_GROUP_NAME];
  char groups[1][MAX_GROUP_NAME];
  int max_groups, ngroups, true_length, max_len;
  int16 mess_type;
  char *msg;
  long long slot;
  struct message_function_init *f; 
  my_bool use_timeout = 0;
  fd_set readfds, nullfds;
  
  /* The "result" buffer passed in is 255 bytes long.  If your result
    fits in this, you don't have to worry about allocating.  */
  max_len = 255;
  msg = result;

  if(args->args[0] == NULL) {
    *error = 1 ;
    return 0;
  }
  slot = *((long long *) args->args[0]);
  if( bad_recv_slot(slot)) 
    goto error_return;
  spread_pool[slot].status = SPREAD_CTX_RECV_MESG;      
  max_groups = 1;
  
  if((args->arg_count == 2) && (args->args[1])) { /* timeout option */
    f = (struct message_function_init *) initid->ptr;
    if(! f->set_options)
    {
      initialize_options(f->options);
      if(parse_api_options(f->options, OPF_timeout, & (f->set_options), args->args[1])) {
        *error = 1;
        return 0;
      }
    }
    if(f->set_options & OPF_timeout) {
      if(! ascii_to_timeval(f->options[OP_timeout].value,
                            f->options[OP_timeout].value_len, & (f->timeval))) {
        *error = 1;
        return 0;
      }
    use_timeout = 1;
    }
  }

  if(use_timeout) {
    FD_ZERO(& readfds);
    FD_ZERO(& nullfds);
    FD_SET( spread_pool[slot].mbox , & readfds);
    
    if( ! select ( spread_pool[slot].mbox + 1, & readfds, & nullfds, & readfds, 
                   & f->timeval)) {
      /* timeout expired; no message. */
      *is_null = 1;
      return NULL;
    }
  }

  get_message:  
  svctype = 0;

  get_long_message:
/*  int SP_receive(    
      mailbox   mbox,   service   *service_type,  char sender[MAX_GROUP_NAME], 
      int   max_groups,  int   *num_groups,   char groups[][MAX_GROUP_NAME],  
      int16  *mess_type, int *endian_mismatch, int max_mess_len, char *mess);
*/
  rcv = SP_receive(spread_pool[slot].mbox, &svctype, sender, max_groups, &ngroups,
                 groups, &mess_type, &true_length, max_len, msg);

  if(rcv == GROUPS_TOO_SHORT) {
    svctype = DROP_RECV;
    goto get_long_message;
  }
  if(rcv == BUFFER_TOO_SHORT) {
    max_len = - true_length;
    msg = malloc(max_len);
    if(msg == NULL) goto error_return; 
    if(UDF_DEBUG)
      fprintf(stderr,"recv_message: malloc, buffer too short\n");
    goto get_long_message;
  }
  if(rcv < 0) 
    goto error_return ;

  spread_pool[slot].status = SPREAD_CTX_JOINED;
  spread_pool[slot].n.recv++;
  
  if( Is_self_leave(svctype)) 
    goto self_leave;
    
  if( Is_membership_mess(svctype))
    goto get_message;

  if ( Is_regular_mess(svctype) && rcv > -1) {
    *length = rcv;
    return msg;
  }
  
  error_return:
  *error = 1;
  if(rcv == CONNECTION_CLOSED) 
    handle_spread_disconnect(rcv,slot);

  if(rcv == ILLEGAL_SESSION)
    handle_illegal_session(rcv,slot);

  self_leave:
  *is_null = 1;
  disconnect_recv_slot(slot);
  
  return NULL;
}


void recv_mesg_deinit(UDF_INIT *initid) {
  struct message_function_init *f; 

  if(initid->ptr) {
    f = ( struct message_function_init * ) initid->ptr;
    if(f->options)
      free( f->options );
    free(f);
  }
  free(initid->ptr);
}


/* SQL track_memberships(), 
   returns an integer slot number from the receive pool
*/
my_bool track_memberships_init(UDF_INIT *initid, UDF_ARGS *args, char *err_msg)
{
  int s, err;

  initid->maybe_null =  0;
  if(args->arg_count != 1) {
    strncpy(err_msg, "track_memberships(): wrong number of arguments",MYSQL_ERRMSG_SIZE);
    return 1;
  }
  args->arg_type[0] = STRING_RESULT;

  pthread_once(& init_group_tables_once, initialize_group_tables);

  if(! send_pool_is_initialized) {
    err = initialize_send_pool();
    if(err != ACCEPT_SESSION) {
      strncpy(err_msg,spread_diagnostic(err),MYSQL_ERRMSG_SIZE);
      return 1;
    }
  }
    
  if(   args->args[0]
     && ((s = group_table_op(OP_LOOKUP,& tracked_groups,0,args->args[0])) > 0)) {
       sprintf(err_msg,"Message handle %d is already tracking memberships "
               "for group \"%s\"\n",s,args->args[0]);
      return 1;
  }
  
  return 0;
}


long long track_memberships(UDF_INIT *initid, UDF_ARGS *args, 
                          char *is_null, char *error)
{
  register int err = 0;
  int slot;
  pthread_attr_t attr;
  pthread_t tid;

  if((args->args[0] == NULL) 
     || (group_table_op(OP_LOOKUP, & tracked_groups,0,args->args[0]) > 0))   {
    *error = 1 ;
    return 0;
  }
  
  slot = get_recv_pool_connection(NULL);
  if (slot < 0) *error = 1;
  
  if(! *error) {
    /* Join a group */
    /* int SP_join( mailbox mbox, const char *group); */
    err = SP_join(spread_pool[slot].mbox, args->args[0]);
    if(err < 0)
      *error = 1;
  }
  
  if(! *error) {
    spread_pool[slot].status = SPREAD_CTX_JOINED;
    strncpy(spread_pool[slot].name.group, args->args[0], args->lengths[0]);
  }
  
  /* Launch the memberships thread */

  pthread_attr_init(& attr);
  pthread_attr_setdetachstate(& attr, PTHREAD_CREATE_DETACHED);
  err = pthread_create(&tid, & attr, memberships_thread, (void *) slot);
  
  if(UDF_DEBUG) fprintf(stderr,"Created memberships thread; return code %d\n",
                        err);

  return (long long) slot;
}  


/* SQL leave_mesg_group 
   returns 1 for success, 0 for failure, NULL if argument is NULL
*/
my_bool leave_mesg_group_init(UDF_INIT *initid, UDF_ARGS *args, char *err_msg)
{
  long long slot;
  
  if(args->arg_count != 1) {
    strncpy(err_msg,"leave_mesg_group(): wrong number of arguments",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }
  args->arg_type[0] = INT_RESULT;

  if(args->args[0]) {
    slot = *((long long *) args->args[0]);
    if(bad_recv_slot(slot) || spread_pool[slot].status < SPREAD_CTX_CONNECTED) 
    {
      strncpy(err_msg,"leave_mesg_group(): invalid message handle",
              MYSQL_ERRMSG_SIZE);
      return 1;
    }
  }

  return 0;
}


long long leave_mesg_group(UDF_INIT *initid, UDF_ARGS *args, 
                          char *is_null, char *error)
{
  long long slot;
  int ret;

  if(args->args[0] == NULL) {
    *is_null = 1 ;
    return 0;
  }
  
  slot = *((long long *) args->args[0]);

  if(bad_recv_slot(slot)) *error = 1;
  else 
    ret = SP_leave(spread_pool[slot].mbox,spread_pool[slot].name.group);
  
  if(ret) *error = 1;
  else
    group_table_op(OP_DELETE, & private_names, slot, spread_pool[slot].name.private);
    
  return (long long) (! *error);
}  


/* SQL mesg_handle
   returns integer handle matching query, or NULL
*/

my_bool mesg_handle_init(UDF_INIT *initid, UDF_ARGS *args, char *err_msg)
{
  opt_parser_return r;
  option_list *Options = NULL;
  unsigned int set_options;
     
  initid->maybe_null = 1;
  if(args->arg_count != 1) {
    strncpy(err_msg,"mesg_handle(): wrong number of arguments",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }
 
  Options = malloc(sizeof(all_api_options));
  initialize_options(Options);
  args->arg_type[0] = STRING_RESULT;
  initid->ptr = (char *) Options;
 
  if(args->args[0]) { 
    r = parse_api_options(Options, (OPF_name | OPF_join | OPF_track),
                          &set_options, args->args[0]);
    if(r) {
      OPTIONS_ERROR("mesg_handle",r);
      return 1;
    }
    else
      STORE_SET_OPTIONS = set_options;
  }

  return 0;
}


long long mesg_handle(UDF_INIT *initid, UDF_ARGS *args, 
                      char *is_null, char *error)
{
  option_list *Options;
  unsigned int set_options;
  int slot;
  char s[MAX_GROUP_NAME];

  if((args->args[0] == NULL) || (initid->ptr == NULL)) {
    *error = 1 ;
    return 0;
  }  
  Options = (option_list *) initid->ptr;

  if(STORE_SET_OPTIONS) 
    /* The query was a static string; mesg_handle_init parsed it */
    set_options = STORE_SET_OPTIONS;  
  else {
    /* Not a static options string; parse it every time */
      initialize_options(Options);
      if(parse_api_options(Options, (OPF_name | OPF_join | OPF_track),
                           &set_options, args->args[0])) {
        *error = 1;
        return 0;
    }
  }
  
  if(set_options & OPF_track) {
    strcpy(s, "");
    strncat(s, Options[OP_track].value, Options[OP_track].value_len);
    slot = group_table_op(OP_LOOKUP, & tracked_groups,0,s);
    if(slot > 0) return (long long) slot;
  }
  else if(set_options & OPF_join) {
    strcpy(s, "");
    strncat(s, Options[OP_join].value, Options[OP_join].value_len);
    for(slot = RECV_POOL ; slot < POOL_SIZE ; slot++) 
      if(((spread_pool[slot].status == SPREAD_CTX_JOINED) ||
          (spread_pool[slot].status == SPREAD_CTX_RECV_MESG))
        && (! strcmp(spread_pool[slot].name.group,s)))
          return (long long) slot;
  }
  else if(set_options & OPF_name) {
    strcpy(s, "");
    strncat(s, Options[OP_name].value, Options[OP_name].value_len);
    /* First try a lookup for an exact match */
    slot = group_table_op(OP_LOOKUP, & private_names,0,s);
    if(slot > -1) return (long long) slot;
    /* Then try a match on the part of the name between # signs */
    strcat(s, "#");
    for(slot = RECV_POOL ; slot < POOL_SIZE ; slot++) 
      if((spread_pool[slot].status > SPREAD_CTX_CONNECTING)
       && (! strncmp(spread_pool[slot].name.private + 1, s, Options[OP_name].value_len + 1 )))
        return (long long) slot;
  }

  *is_null = 1;
  return (long long) 0;
}  


void mesg_handle_deinit(UDF_INIT *initid) {
  free(initid->ptr);
}


my_bool mesg_status_init(UDF_INIT *initid, UDF_ARGS *args, char *err_msg)
{
  if(args->arg_count > 1) {
    strncpy(err_msg,"mesg_status(): wrong number of arguments", MYSQL_ERRMSG_SIZE);
    return 1;
  }
  
  initid->maybe_null = 0;
  initid->max_length = STATUS_REPORT_SIZE;
  initid->ptr = malloc(STATUS_REPORT_SIZE); 
  return 0;
}


char * mesg_status(UDF_INIT *initid, UDF_ARGS *args, char *result,
                 unsigned long *length, char *is_null, char *error)
{
  int sz = 0;
  long long slot;
  char *p;
  
  p = initid->ptr;
  
  if(args->arg_count == 1 && args->args[0]) {
    slot =  *((long long *) args->args[0]);
    if((slot < 0) || (slot >= POOL_SIZE)) {
      *error = 1;
      return NULL;
    }
    else if(spread_pool[slot].status == SPREAD_CTX_FREE) {
      *length = 0;
      return "";
    }
    else if(slot < SEND_POOL_SIZE) {
      status__send_pool(p, &sz, slot);
      status__outboxes(p, &sz, slot);
    }
    else if(slot < POOL_SIZE) {
      if(spread_pool[slot].status == SPREAD_CTX_TRACK_MEMBERSHIPS)
        status__group_table(p, &sz, slot);
      else 
        status__recv_pool(p, &sz, slot) ;
    }
    else {
      *error = 1;
      return NULL;
    }
  }
  else {    /* ALL SLOTS */
    status__global(p, &sz);
    status__send_pool(p, &sz, -1);
    status__outboxes(p, &sz, -1);
    status__recv_pool(p, &sz, -1);
    status__group_table(p, &sz,-1);
  }

  *length = sz;
  return initid->ptr;
}


void mesg_status_deinit(UDF_INIT *initid) {
  free(initid->ptr);
}


long long is_a_member( struct membership *memb, sequence_number seq, char *member) {
  char *p, *q;
  struct member_list *v;
  long long return_value = 0;

  rw_rdlock( & memb->list_lock); 
    v = memb->vers;
    
    /* traverse the list of membership lists, finding the one that corresponds 
       with the given message sequence number */
    /* (If the memberships thread has shut down, v may already be a null pointer) */ 

    while(v && 
            (! (seq > v->valid_start && (seq < v->valid_end || ! v->valid_end))
          )) v = v->earlier;

    if(v == NULL) return 0;
    
    p = (char *) v->members;
    q = (char *) ( p + (MAX_GROUP_NAME * v->n_members));

    /* member could be a full private name, like "#user1#madison",
       which Spread will report to you.  Or it might just be "user1",
       which you have requested in SP_connect().  We accept either form,
       and we do this by:
       skipping over the first character of p (known to be a '#')
       skipping the first character of member if it is a '#'
       doing a leftmost-substring match: does "p" begin with "member"?
    */
	
    if(*member == '#') member++;
	
    while(p < q) 
    { 
      if(strncmp(p+1,member,strlen(member)) == 0) 
      {
          return_value = 1;
          break;
      }
      p = (char *)  ( p + MAX_GROUP_NAME);
    }
  
  rw_unlock( & memb->list_lock);
  return return_value;
}



static inline void memberships_new_list (struct membership *m, int n, char *members) {
  struct member_list *ver;
  
  /* Update the membership list. */
  /* The memberships thread calls this code when it 
     (a) gets a regular membership message, or
     (b) must duplicate the current membership list because the seq no. has wrapped. 
  */

  /* TO DO: verify the case where the sequence number has wrapped 
     around during the time that the membership was transitional 
     (this has worked correctly in some tests)
  */

  ver = (struct member_list *) malloc(sizeof(struct member_list));
  ver->valid_start = (m->trans.state ? m->vers->valid_end : m->mesg_sequence);
  ver->valid_end = 0;
  ver->n_members = n;
  ver->members = malloc(n * MAX_GROUP_NAME);
  memcpy(ver->members, members, (n * MAX_GROUP_NAME));
  /* ver->later for the most recent version points back to the tail of the list.
     Special case: ver->later of the only version points to itself. */
  if(m->vers) ver->later = m->vers->later;
  else ver->later = ver;

  m->n_list_versions++;
  m->list_mem_used += sizeof(struct member_list) + (n * MAX_GROUP_NAME);
         
  rw_wrlock(& m->list_lock);
    if(! m->vers) {
      /* this is the initial version of the list */
      ver->earlier = NULL;
    }
    else {
      ver->earlier = m->vers ; 
      m->vers->later = ver;
      if(! m->trans.state) m->vers->valid_end = m->mesg_sequence - 1;
    }
    m->vers = ver;
  rw_unlock(& m->list_lock);
}


/* Expire unneeded old versions so that we don't leak memory. 
 * Return the number of received but unclaimed messages.
 */
static int memberships_purge_old_lists(int slot) {
  struct membership *m;
  struct member_list *ver, *new_list_tail;
  sequence_number max_needed_message = 0;
  int i, j, n = 0;
  
  m = spread_pool[slot].m;

  /* If a pending message exists that will require an older version of the 
     memberships list, we know that it is marked as BOX_RECVD. */
  for(i = 0 ; i < SEND_POOL_SIZE ; i++) {
    pthread_mutex_lock(& outboxes[i].lock);
      for(j = 0 ; j <= outboxes[i].max_used_tag ; j++) {
        if(   outboxes[i].mesg[j].mthread_slot == slot 
              && outboxes[i].mesg[j].status == BOX_RECVD
              && outboxes[i].mesg[j].sequence > max_needed_message) {
          
            max_needed_message = outboxes[i].mesg[j].sequence;
            n++;
        }
      }
    pthread_mutex_unlock(& outboxes[i].lock); 
  }

  rw_wrlock( & m->list_lock);  
    /* Start at the list tail and walk toward the head  */
    for(ver = m->vers->later ; ver->later != m->vers ; ver = ver->later) {
      if((ver->valid_start > max_needed_message) && (ver->valid_end != 0)) {
          /* Purge */
          new_list_tail = ver->later;
          new_list_tail->earlier = NULL;
          m->vers->later = new_list_tail;
          m->n_list_versions --;
          
          free(ver->members);
          m->list_mem_used -= (ver->n_members * MAX_GROUP_NAME) ;

          free(ver);
          m->list_mem_used -= (sizeof(struct member_list));
      }
      else break;
    }
  rw_unlock( & m->list_lock);
  
  return n;
}



#define TRANSITIONAL_STATE ctx->m->trans.state

void * memberships_thread(void * a) {
  int slot, rcv, i; 
  struct spread_context *ctx;
  struct member_list *ver;
  char *msg = NULL;
  void *groups = NULL;
  register void *t;
  int max_groups = 100;
  int max_len = 4096;
  int ngroups, endian_mismatch, sender_slot;
  int16 mess_type, tag;
  service svctype;
  char sender[MAX_GROUP_NAME];
  struct mesg_queue_rec q;
    
  slot = (int) a ;
  ctx = & spread_pool[slot];
  
  /* buffers to receive the message text and group lists */
  msg = malloc(max_len);
  groups = calloc(max_groups , MAX_GROUP_NAME); 
  
  /* Create a membership structure.
     Rely on calloc() to initialize all fields to 0. */
  ctx->m = (struct membership *) calloc(1 , sizeof(struct membership));
  if(!( msg && groups && ctx->m)) goto exit_point_2;
  my_rwlock_init(& ctx->m->list_lock, NULL);

  group_table_op(OP_CREATE, & tracked_groups,slot, ctx->name.group);

  /* Work Queue for membership transitions */
  ctx->m->trans.max_size = TRANSITIONAL_QUEUE_SIZE;
  ctx->m->trans.q = calloc( ctx->m->trans.max_size, sizeof(struct mesg_rec));
  ctx->m->trans.size = 0;
  TRANSITIONAL_STATE = 0;

  ctx->status = SPREAD_CTX_TRACK_MEMBERSHIPS;
  
  while(1) {
    svctype = 0;
    /*  int SP_receive(mailbox   mbox, service   *service_type,  
                       char sender[MAX_GROUP_NAME], int   max_groups,  
                       int   *num_groups,  char groups[][MAX_GROUP_NAME],  
                       int16  *mess_type, int *endian_mismatch, 
                       int max_mess_len, char *mess);
    */
    rcv = SP_receive(ctx->mbox, &svctype, sender, max_groups, &ngroups,
                     groups, &mess_type, &endian_mismatch, max_len, msg);
                     
    ctx->m->mesg_sequence++;

    if(! ctx->m->mesg_sequence) {
      /* The message sequence number has wrapped around back to zero... */
      memberships_new_list(ctx->m, ctx->m->vers->n_members, ctx->m->vers->members);
      /* which is not an allowable sequence number, because it has a special
         meaning as a return value from delivery_broker(). */
      ctx->m->mesg_sequence++;
    }

    if(Is_regular_mess(svctype)  &&  ((rcv > -1) || (rcv == BUFFER_TOO_SHORT))) {

#ifdef UDF_DEBUG
      if(!(strncmp(msg,"__SIMULATED_TRANSITIONAL_MEMB_MESG",34))) 
      {
        fprintf(stderr,"memberships thread received simulated transitional message\n");
        goto handle_transitional_message;
      }
#endif   
      /* Test to see if this was a guaranteed message sent by a sender on this
         server */
      if((mess_type >= base_tag) && (mess_type <= base_tag + OUT_BOX_SIZE) 
          && ((sender_slot = group_table_op(OP_LOOKUP, & private_names, 0, sender)) 
              != GROUP_NOT_FOUND))  
      {
        tag = mess_type - base_tag;
        if(TRANSITIONAL_STATE)
        {
          /* The group membership is in transition, so we must queue 
             delivery receipts until a regular membership message
             has arrives and clarified the situation
          */
          ctx->m->trans.q[ctx->m->trans.size].seq = ctx->m->mesg_sequence;
          ctx->m->trans.q[ctx->m->trans.size].tag = tag;
          ctx->m->trans.q[ctx->m->trans.size].outbox = & outboxes[sender_slot];          
          ctx->m->trans.size++;
          if(ctx->m->trans.size == ctx->m->trans.max_size) 
          {
            ctx->m->trans.max_size *= 2;
            ctx->m->trans.q = realloc( ctx->m->trans.q ,
              ctx->m->trans.max_size * sizeof(struct mesg_rec));
          }
        }
        else 
        {
          /* The group membership is known, so we register receipt of this
             message with the delivery broker */
          delivery_broker(BOX_RECVD, & outboxes[sender_slot], 
                          tag, ctx->m->mesg_sequence);
        }
      }
      continue;
    }
    
    if(rcv < 0) {
      if(rcv == GROUPS_TOO_SHORT) {
        max_groups = (- ngroups) + 10;
        t = malloc(max_groups * MAX_GROUP_NAME);
        groups = t;
        continue;
      }
      else if(rcv == CONNECTION_CLOSED) {
        /* 
           We have become disconnected from the local Spread daemon.
           It's probably dead. 
           TO DO: set a flag here so that we can try to reconnect to Spread 
           automatically when it comes back.
        */
        SP_error(rcv);
        goto exit_point_1;
      }
      else {
        SP_error(rcv);
        continue;
      }
    } /* rcv < 0 */
    
    if( Is_transition_mess(svctype)) 
    {
      handle_transitional_message:
      TRANSITIONAL_STATE = 1;
      ctx->m->vers->valid_end = ctx->m->mesg_sequence;
      continue;
    }
    if( Is_reg_memb_mess(svctype)) 
    {
      ctx->m->n_changes++;
      memberships_new_list(ctx->m, ngroups, groups);
      if(TRANSITIONAL_STATE) {
        /* The new membership list applies to all regular messages received 
           since the transitional state began, ending the transition.
        */
        while(ctx->m->trans.size) {
          ctx->m->trans.size--;
          q = ctx->m->trans.q[ctx->m->trans.size];
          delivery_broker(BOX_RECVD, q.outbox, q.tag, q.seq);
        }
        TRANSITIONAL_STATE = 0;
      }

      if(   ctx->m->list_mem_used > LIST_MEM_IS_BIG 
         && ctx->m->n_list_versions > 3)
          memberships_purge_old_lists(slot);

      continue;
    }
    if( Is_self_leave(svctype)) 
      goto exit_point_1;

    else fprintf(stderr,"STRANGE MESSAGE %d %d \n",svctype,rcv);
  }  /* while(1) */


  exit_point_1:

  /* Some outboxes may be waiting for BOX_RECVD on -future- messages, 
  so notify them all that we are closing down */
  for(i = 0 ; i < SEND_POOL_SIZE ; i++) 
    delivery_broker(BOX_CLOSED, & outboxes[i], slot, ctx->m->mesg_sequence + 1);

  /*  memberships_purge_old_lists() returns the number of still-pending receipts */
  i = rcv = 0;
  do {
    rcv = memberships_purge_old_lists(slot);
    if(rcv) sleep(1);
    i++;
  } while(rcv && (i < 60));   
  if(i == 60) {
    fprintf(stderr,"- Message API: Memberships thread (handle %d).\n"
            "- Timed out waiting for senders to pick up receipt%s for %d message%s;\n"
            "- must exit without freeing memory.\n",
            slot, (rcv == 1 ? "" : "s") , rcv , (rcv == 1 ? "" : "s"));
    ctx->status = SPREAD_CTX_ZOMBIE;
    pthread_exit(NULL);
  }
    
  /* At this point only the most-recent membership list remains; free it now. */
  ver = ctx->m->vers ;
  ctx->m->vers = NULL;
  free(ver->members);
  free(ver);

   
  exit_point_2:

  if(UDF_DEBUG) 
   fprintf(stderr, "Memberships thread exiting (handle %d; group: %s).\n",
           slot,ctx->name.group);     
  group_table_op(OP_DELETE, & tracked_groups,slot,ctx->name.group);
  if(groups) free(groups);
  if(msg) free(msg);
  ctx->status = SPREAD_CTX_CONNECTED;
  if(ctx->m) {
    if(ctx->m->trans.q) free(ctx->m->trans.q);
    rwlock_destroy(& ctx->m->list_lock);
    free(ctx->m);
  }
  disconnect_recv_slot(slot);
  pthread_exit(NULL);
}
                                 

void initialize_group_tables() {
  my_rwlock_init(& tracked_groups.lock,NULL);
  my_rwlock_init(& private_names.lock,NULL);
}


/* group_table_op()
   Operations are OP_LOOKUP, OP_CREATE, and OP_DELETE
   Returns a slot number, or GROUP_IS_DUPLICATE, or GROUP_NOT_FOUND
*/

int group_table_op(enum hash_op op, struct group_table *table, 
                   int slot, char *group) {
  unsigned int h = 0;
  unsigned char *p;
  struct name_list *g;
    
  /* Calculate Hash */
  for (p = (unsigned char *) group ; *p != '\0' ; p++) 
    h = 31 * h + *p;
  h = h % GROUP_TABLE_SIZE ;

  if(op == OP_LOOKUP)
    rw_rdlock(& table->lock);
  else
    rw_wrlock(& table->lock);

  for(g = table->names[h] ; g != NULL ; g = g->next) {
    if ((strcmp(g->name, group) == 0)) {
      /* Found a match. */
      switch(op) {
        case OP_DELETE:
          table->names[h] = g->next ;
          free(g);
          rw_unlock(& table->lock);
          return 0;
        case OP_LOOKUP:
          rw_unlock(& table->lock);
          return g->slot;
        case OP_CREATE:
          rw_unlock(& table->lock);
          return GROUP_IS_DUPLICATE;
      }
    }
  }
  if(op == OP_CREATE) {
    g = (struct name_list *) malloc(sizeof(struct name_list));
    g->slot = slot;
    g->next = table->names[h];
    g->name = group;
    table->names[h] = g;
    rw_unlock(& table->lock);
    return 0;
  }
  rw_unlock(& table->lock);
  return GROUP_NOT_FOUND;
}



void status__global(char *p, int *sz) {
  report(p,sz,"\n=================  Globals  \n");
  report(p,sz,"  %d message handles in send pool, %d handles in receive pool\n",
         SEND_POOL_SIZE, RECV_POOL_SIZE);
  report(p,sz,"  Base tag for guaranteed messages: %d\n", base_tag);
  report(p,sz,"  %d outboxes per sender.\n", OUT_BOX_SIZE);
  report(p,sz,"\n");
}


char * status__slot(int i) {
  switch(spread_pool[i].status) {
    case SPREAD_CTX_FREE:
      return "Free";
    case SPREAD_CTX_ZOMBIE:
      return "Zombie";
    case SPREAD_CTX_DISCONNECTED: 
      return "Disconnected";
    case SPREAD_CTX_CONNECTING:
      return "Connecting";
    case SPREAD_CTX_CONNECTED:
      return "Connected";
    case SPREAD_CTX_JOINED:
      return "Group Joined";
    case SPREAD_CTX_TRACK_MEMBERSHIPS:
      return "Tracking Memberships";
    case SPREAD_CTX_RECV_MESG:
      return "Receiving";
    default:
      return "UNKNOWN";
  }
}

char * status__box(outbox_tag t) {
  switch(t) {
    case BOX_CLEAR :
      return "Clear";
    case BOX_SENDING: 
      return "Sending";
    case BOX_SENT:
      return "Sent";
    case BOX_RECVD:
      return "Recvd";
    case BOX_NOT_RECVD:
      return "Not_Recvd";
    case BOX_CLOSED:
      return "Closed";
    default:
      return "unknown";
  }
}


        
void status__send_pool(char *p, int *sz, int slot) {
  int i;

  report(p,sz,"\n=================  Send Pool  \n");
  report(p,sz,"Private Name                 Handle       Sent MaxQ            Status\n");
  report(p,sz,"---------------------------------------------------------------------\n");
  for(i = 0 ; i < SEND_POOL_SIZE ; i++)
    if((spread_pool[i].status > SPREAD_CTX_FREE) 
       && ( (slot == -1) || (i == slot) ))
        report(p,sz,"%-32s %2d %10d %3d%20s\n",spread_pool[i].name.private, i,
               spread_pool[i].n.sent, outboxes[i].max_used_tag, status__slot(i));
  report(p,sz,"\n");             
}


void status__recv_pool(char *p, int *sz, int slot) {
  int i;
  char names[(2 * MAX_GROUP_NAME) + 4];
  
  report(p,sz,"\n=================  Receive Pool  \n");
  report(p,sz,"Group : Name                        Handle      # Rcvd         Status\n");  
  report(p,sz,"---------------------------------------------------------------------\n");
  for(i = RECV_POOL ; i < POOL_SIZE ; i++) {
    if(spread_pool[i].status > SPREAD_CTX_FREE
       && spread_pool[i].status != SPREAD_CTX_TRACK_MEMBERSHIPS
       && (slot == -1 || slot == i)) {
        strcpy(names,spread_pool[i].name.group);
        strcat(names," : ");
        strcat(names,spread_pool[i].name.private);
        report(p,sz,"%-37s   %2d  %10d%15s \n",names, i,
               spread_pool[i].n.recv,status__slot(i));
    }
  }
  report(p,sz,"\n");            
}


void status__group_table(char *p, int *sz, int slot) {
  int h, n;
  struct name_list *g;
  char *q, *r;
  struct spread_context *ctx;

  report(p,sz,"\n=================  Group Table  \n");
  report(p,sz,"Group                        Handle   Messages Members Changes Lists Memory\n");
  report(p,sz,"---------------------------------------------------------------------------\n");
  for(h = 0 ; h < GROUP_TABLE_SIZE ; h++) {
    for(g = tracked_groups.names[h] ; g != NULL ; g = g->next) {
      if(slot != -1 && slot != g->slot) continue;
      ctx = & spread_pool[g->slot];
      report(p,sz,"%-32s %2d %10d %7d %7d %5d %6d\n",
             g->name, g->slot, ctx->m->mesg_sequence, ctx->m->vers->n_members, 
             ctx->m->n_changes, ctx->m->n_list_versions, ctx->m->list_mem_used);
      n = 0;
      if(ctx->m->trans.state) {
        report(p,sz,"  -- MEMBERSHIP IS TRANSITIONAL since %d,\n      with %d sent "
               "message%s awaiting confirmation.\n", ctx->m->vers->valid_end, 
               ctx->m->trans.size, (ctx->m->trans.size == 1) ? "" : "s");
      }
      else {
        q = (char *) ctx->m->vers->members;
        for(r = q ; n < ctx->m->vers->n_members ; n++,r = (char *)(r + MAX_GROUP_NAME)){
          if(n % 3 == 2) report(p,sz," %-25s\n",r);
          else report(p,sz," %-25s",r);
        }
        if(n % 3 != 2) report(p,sz,"\n");
      }
    }
  }
}


void status__outboxes(char *p, int *sz, int slot) {
  int i,j;
  outbox_tag t;
  
  report(p,sz,"\n=================  Outboxes \n");
  for(i = 0 ; i < SEND_POOL_SIZE ; i++) 
    if(slot == -1 || slot == i) 
      for(j = 0 ; j < OUT_BOX_SIZE ; j++) 
        if((t = outboxes[i].mesg[j].status) != BOX_CLEAR) 
          report(p,sz,"%d:%d %s%s ; ",i,j,status__box(t),
                 (outboxes[i].mesg[j].flags.timed_out ? " TIMED_OUT" : ""));
  report(p,sz,"\n");
}
 

void report(char *p, int *sz, char *fmt, ... ) {
  va_list args;
  
  va_start(args,fmt);
  *sz += vsnprintf((p + *sz), STATUS_REPORT_SIZE - *sz, fmt, args);
  va_end(args);
}

