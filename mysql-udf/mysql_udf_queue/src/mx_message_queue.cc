/*
 * mx_message_queue.c
 *
 *  Created on: 2012-7-31
 *      Author: DenoFiend
 */

#include "mx_message_queue.h"
#include "pbc/online.pb.h"
#include "beanstalk_client/BeanstalkClient.h"

#ifdef __cplusplus
extern "C"
{
#endif

my_bool mx_push_queue_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 8)
	{
		// wrong arg count.
		strmov(
				message,
				"Wrong arguments to mx_push_queue;  Use the [id] [mx2] [mx3] [mxa] [mxios] [mxmac] [online_flag] [grade]");
		return 1;
	}

	return 0;
}
void mx_push_queue_deinit(UDF_INIT *initid)
{

}

int serializeToString(UDF_ARGS *args, std::string&encodeString, char *result,
		unsigned long *length, char *null_value, char *error)
{
	online::message::maxthon::cn::Online online;

	// [id]
	int32_t id = atoi(args->args[0]);
	online.set_id(id);

	// [mx2]
	int32_t mx2 = atoi(args->args[1]);
	online.set_mx2(mx2);

	// [mx3]
	int32_t mx3 = atoi(args->args[2]);
	online.set_mx3(mx3);

	// [mxa]
	int32_t mxa = atoi(args->args[3]);
	online.set_mxa(mxa);

	// [mxios]
	int32_t mxios = atoi(args->args[4]);
	online.set_mxios(mxios);

	// [mxmac]
	int32_t mxmac = atoi(args->args[5]);
	online.set_mxmac(mxmac);

	// [online_flag]
	int32_t online_flag = atoi(args->args[6]);
	online.set_online_flag(online_flag);

	// [grade]
	int32_t grade = atoi(args->args[7]);
	online.set_grade(grade);

	bool f = online.SerializeToString(&encodeString);
	if (!f)
	{
		*null_value = 1;
		return 0;
	}
	else
	{
		return 1;
	}
}
char *mx_push_queue(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *length, char *null_value, char *error)
{

	// 1. google procotolbuf
	std::string encodeStr;
	int res = serializeToString(args, encodeStr, result, length, null_value,
			error);

	if (1 != res)
	{
		return 0;
	}
	// 2. set beanstalkd

	Beanstalk::BeanstalkClient client("127.0.0.1", 11300);

	printf(">>> beanstalk client set(%s)\n", encodeStr.c_str());

	int id = client.put(encodeStr.data(), encodeStr.size());

	sprintf(result, "put beanstalk succes, job id(%d)", id);
	*length = strlen(result);

	return result;
}

my_bool reverse_mx_push_queue_init(UDF_INIT *initid, UDF_ARGS *args,
		char *message)
{
	my_bool res;
	return res;
}
void reverse_mx_push_queue_deinit(UDF_INIT *initid)
{

}
char *reverse_mx_push_queue(UDF_INIT *initid, UDF_ARGS *args, char *result,
		unsigned long *length, char *null_value, char *error)
{
	return result;
}

#ifdef __cplusplus
}
#endif
