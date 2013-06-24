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


/* 
The default SPREAD_NAME of "4803" will connect to a Spread daemon running 
on the local server at port 4803.  It is STRONGLY recommended that you 
run a Spread daemon on each MySQL Server.
*/

#ifndef SPREAD_NAME 
#define SPREAD_NAME "4803"
#endif

/* 
 RECV_POOL_SIZE defines a limit on the possible number of simultaneously 
 running mesg_recv() and track_memberships() calls in MySQL.
 SEND_POOL_SIZE does not impose a limit on the number of simultaneous 
 sends, though you may get better concurrency (and will use more memory) 
 by increasing it.
*/

#ifndef SEND_POOL_SIZE
#define SEND_POOL_SIZE 8
#endif

#ifndef RECV_POOL_SIZE
#define RECV_POOL_SIZE 56
#endif

/* 
  Print debugging messages to the mysqld error log?
*/
#define UDF_DEBUG 1

/*
  Maximum possible size of the output from the mesg_status() command.
  (in bytes)
*/
#define STATUS_REPORT_SIZE 16384

/*
  Size of the hash table that stores the names of joined groups 
*/
#define GROUP_TABLE_SIZE 128

/* 
  How many outboxes (for guaranteed message delivery)
  does each sender have? 
*/
#define OUT_BOX_SIZE 150 

/*
  How many messages can you handle while membership is transitional?
  (This is not a hard limit; if you reach it, the memberships thread
   will simply allocate a bigger queue).
*/
#define TRANSITIONAL_QUEUE_SIZE 2048


/* 
  How big do you let old membership lists get before you try to purge them?
 (in bytes)
*/
#define LIST_MEM_IS_BIG 16384


/***** ============= 
  This is the end of the compile-time configurable values.
  Nothing user-friendly below this line.
====== *************/

#include <my_global.h>
#include <my_pthread.h>
#include <mysql_com.h>

#include <string.h>
#include <stdarg.h>

#include <sp.h>


#define udf_assert(x) { \
  (x) || fprintf(stderr,"!! Message UDF assertion failed at line %d !! \n",__LINE__);\
}

/* HAVE_LRAND48 is from my_global.h */
#ifdef HAVE_LRAND48
#define get_rand lrand48
#define seed_rand srand48
#else
#define get_rand rand
#define seed_rand srand
#endif


/*
In actuality the send pool and recv pool are a single array;
the recv pool starts at the end of the send pool.
*/
#define POOL_SIZE ( SEND_POOL_SIZE + RECV_POOL_SIZE )
#define RECV_POOL SEND_POOL_SIZE

enum hash_op { 
  OP_CREATE,
  OP_LOOKUP,
  OP_DELETE
};

enum { /* errors returned from group_table_op() */
  GROUP_NOT_FOUND = -1 ,
  GROUP_IS_DUPLICATE = -2
};

enum outbox_tag {
  BOX_CLEAR,
  BOX_SENDING,
  BOX_SENT,
  BOX_RECVD,
  BOX_NOT_RECVD,
  BOX_CLOSED
};
typedef enum outbox_tag outbox_tag;

enum ctx_status {  
  SPREAD_CTX_FREE , 
  SPREAD_CTX_ZOMBIE, 
  SPREAD_CTX_DISCONNECTED, 
  SPREAD_CTX_CONNECTING,
  SPREAD_CTX_CONNECTED,
  SPREAD_CTX_JOINED,
  SPREAD_CTX_TRACK_MEMBERSHIPS,
  SPREAD_CTX_RECV_MESG
};
typedef enum ctx_status ctx_status;

#ifdef DEBUG_SEQ_WRAP
typedef unsigned char sequence_number;
#else
typedef unsigned long sequence_number;
#endif


/* struct spread_context
   The connection pool is an array of these;
   this is the principal data structure in this code.
*/
struct spread_context {
  mailbox mbox;
  enum ctx_status status;
  union {
    unsigned int sent;
    unsigned int recv;
  } n;
  struct membership *m;     /* used only by track_memberships() */
  struct outbox *outbox;    /* used only by senders */
  struct {
    char group[MAX_GROUP_NAME];
    char private[MAX_GROUP_NAME];
  } name;
};


/* struct mesg_queue_rec
   When the network state is transitional, the memberships thread tracks 
   received messages in an array of mesg_queue_rec structures
*/
struct mesg_queue_rec {
  sequence_number seq;
  struct outbox *outbox;
  int16 tag;
};


/* struct membership
   The primary data structure used for track_memberships()
*/
struct membership {
  rw_lock_t list_lock;
  struct member_list *vers;
  sequence_number mesg_sequence;
  unsigned int n_changes;
  unsigned int n_list_versions;
  unsigned int list_mem_used ; 
  struct transitional_mesg_queue {
    unsigned int size;
    unsigned int max_size;
    struct mesg_queue_rec *q;
    my_bool state;
  } trans;
};


/* struct member_list
   Holds a particular version of a membership list, valid for messages 
   with sequence numbers from valid_start to valid_end, with links
   to earlier and later versions 
*/
struct member_list {
  sequence_number valid_start;
  sequence_number valid_end; 
  int n_members; 
  void * members;
  struct member_list *earlier;
  struct member_list *later;
};


/* struct mesg_rec
   ctx->outbox has an array of OUT_BOX_SIZE mesg_rec structures
*/
struct mesg_rec {
  sequence_number sequence;
  short mthread_slot;
  outbox_tag status;
  struct {
    unsigned int timed_out : 1 ;
  } flags;
};


/* struct outbox
   head of the data structure used by a sender 
   to track guaranteed sent messages
*/
struct outbox {
  pthread_mutex_t  lock;
  pthread_cond_t   telephone;
  short            max_used_tag;
  unsigned int     n_broadcast;
  struct mesg_rec  mesg[OUT_BOX_SIZE];
};


/* struct group_table
   A hash table
 */
struct group_table {
  rw_lock_t  lock;
  struct name_list *names[GROUP_TABLE_SIZE];
};


/* struct name_list
   An element in a hash chain
 */
struct name_list {
  char *name;
  int len;
  int slot;
  struct name_list *next;
};


/* struct message_function_init
   An explicit structure passed in initid->ptr from a UDF init function
   to a UDF main function 
 */
struct message_function_init {
  struct timeval timeval;
  int slot;
  unsigned int set_options;
  struct spread_context *ctx;
  struct option_list *options;  
  char *text;
};


/*
 *   function prototypes
 */

long long is_a_member( struct membership *, sequence_number, char *);
int group_table_op(enum hash_op, struct group_table *, int, char *);
void * memberships_thread(void * a);
void report(char *p, int *sz, char *fmt, ... );
void initialize_group_tables();
void initialize_outboxes();
sequence_number delivery_broker(outbox_tag, struct outbox *, int16 , sequence_number );
long long send_spread_message(UDF_INIT *, UDF_ARGS *, char *, char *);
long long send_guaranteed_message(UDF_INIT *, UDF_ARGS *, char *, char *);
static inline void disconnect_recv_slot(int);

void status__global(char *p, int *sz);
void status__group_table(char *p, int *sz, int slot);
void status__send_pool(char *p, int *sz, int slot);
void status__recv_pool(char *p, int *sz, int slot);
void status__outboxes(char *p, int *sz, int slot);

