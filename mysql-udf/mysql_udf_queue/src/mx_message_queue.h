/*
 * mx_message_queue.h
 *
 *  Created on: 2012-7-31
 *      Author: DenoFiend
 */

#ifndef MX_MESSAGE_QUEUE_H_
#define MX_MESSAGE_QUEUE_H_

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong; /* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>		/* To get strmov() */
#else
/* when compiled as standalone */
#include <string.h>
#define strmov(a,b) stpcpy(a,b)
#define bzero(a,b) memset(a,0,b)
#define memcpy_fixed(a,b,c) memcpy(a,b,c)
#endif
#endif
#include <mysql.h>
#include <ctype.h>

#ifdef __WIN__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

my_bool mx_push_queue_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void mx_push_queue_deinit(UDF_INIT *initid);
char *mx_push_queue(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *length, char *null_value, char *error);

my_bool reverse_mx_push_queue_init(UDF_INIT *initid, UDF_ARGS *args,
		char *message);
void reverse_mx_push_queue_deinit(UDF_INIT *initid);
char *reverse_mx_push_queue(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *length, char *null_value, char *error);

#ifdef __cplusplus
}
#endif

#endif /* MX_MESSAGE_QUEUE_H_ */
