/*
 * mx_message_queue.c
 *
 *  Created on: 2012-7-31
 *      Author: DenoFiend
 */

#include "mx_message_queue.h"

my_bool mx_push_queue_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	my_bool res;

	return res;
}
void mx_push_queue_deinit(UDF_INIT *initid)
{


}

char *mx_push_queue(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *length, char *null_value, char *error)
{

	return result;
}


my_bool reverse_mx_push_queue_init(UDF_INIT *initid, UDF_ARGS *args,
		char *message);
void reverse_mx_push_queue_deinit(UDF_INIT *initid);
char *reverse_mx_push_queue(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *length, char *null_value, char *error);

