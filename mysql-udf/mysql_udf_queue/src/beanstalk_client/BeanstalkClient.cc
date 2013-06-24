/*
 * RedisClient.cpp
 *
 *  Created on: 2012-3-12
 *      Author: DenoFiend
 */

#include <stdexcept>
#include <sstream>
#include <iostream>
#include "beanstalk_client/BeanstalkClient.h"

using namespace std;

namespace Beanstalk
{

Job::Job()
{
	_id = 0;
}

Job::Job(int id, char *data, size_t size)
{
	_body.assign(data, size);
	_id = id;
}

Job::Job(BSJ *job)
{
	if (job)
	{
		_body.assign(job->data, job->size);
		_id = job->id;
		bs_free_job(job);
	}
	else
	{
		_id = 0;
	}
}

string& Job::body()
{
	return _body;
}

int Job::id()
{
	return _id;
}

/* start helpers */

void parsedict(stringstream &stream, info_hash_t &dict)
{
	string key, value;
	while (true)
	{
		stream >> key;
		if (stream.eof())
			break;
		if (key[0] == '-')
			continue;
		stream >> value;
		key.erase(--key.end());
		dict[key] = value;
	}
}

void parselist(stringstream &stream, info_list_t &list)
{
	string value;
	while (true)
	{
		stream >> value;
		if (stream.eof())
			break;
		if (value[0] == '-')
			continue;
		list.push_back(value);
	}
}

/* end helpers */

BeanstalkClient::~BeanstalkClient()
{
	bs_disconnect(handle);
}

BeanstalkClient::BeanstalkClient(string host, int port) :
		handle(-1), host_(host), port_(port)
{

}

void BeanstalkClient::init()
{
	if (0 < handle)
		return;

	handle = bs_connect((char*) (host_.c_str()), port_);

	if (0 > handle)
		throw runtime_error("unable to connect to beanstalkd at " + host_);
}

bool BeanstalkClient::use(string tube)
{
	init();
	return bs_use(handle, (char*) tube.c_str()) == BS_STATUS_OK;
}

bool BeanstalkClient::watch(string tube)
{
	init();
	return bs_watch(handle, (char*) tube.c_str()) == BS_STATUS_OK;
}

bool BeanstalkClient::ignore(string tube)
{
	init();
	return bs_ignore(handle, (char*) tube.c_str()) == BS_STATUS_OK;
}

int BeanstalkClient::put(string body, int priority, int delay, int ttr)
{
//	logger().trace(">>> BeanstalkClient::put handle(%d), body(%s)\n", handle,
//			body.c_str());

	init();

	int id = bs_put(handle, priority, delay, ttr,
			(char*) (body.data()), body.size());
	return (id > 0 ? id : 0);
}

int BeanstalkClient::put(char *body, size_t bytes, int priority, int delay,
		int ttr)
{
	init();
	int id = bs_put(handle, priority, delay, ttr, body, bytes);
	return (id > 0 ? id : 0);
}

bool BeanstalkClient::del(int id)
{
	init();
	return bs_delete(handle, id) == BS_STATUS_OK;
}

bool BeanstalkClient::reserve(Job & job)
{
	init();
	BSJ *bsj;
	if (bs_reserve(handle, &bsj) == BS_STATUS_OK)
	{
		job = bsj;
		return true;
	}
	return false;
}

bool BeanstalkClient::reserve(Job & job, int timeout)
{
	init();
	BSJ *bsj;
	if (bs_reserve_with_timeout(handle, timeout, &bsj) == BS_STATUS_OK)
	{
		job = bsj;
		return true;
	}
	return false;
}

bool BeanstalkClient::release(int id, int priority, int delay)
{
	init();
	return bs_release(handle, id, priority, delay) == BS_STATUS_OK;
}

bool BeanstalkClient::bury(int id, int priority)
{
	init();
	return bs_bury(handle, id, priority) == BS_STATUS_OK;
}

bool BeanstalkClient::touch(int id)
{
	init();
	return bs_touch(handle, id) == BS_STATUS_OK;
}

bool BeanstalkClient::peek(Job & job, int id)
{
	init();
	BSJ *bsj;
	if (bs_peek(handle, id, &bsj) == BS_STATUS_OK)
	{
		job = bsj;
		return true;
	}
	return false;
}

bool BeanstalkClient::peek_ready(Job & job)
{
	init();
	BSJ *bsj;
	if (bs_peek_ready(handle, &bsj) == BS_STATUS_OK)
	{
		job = bsj;
		return true;
	}
	return false;
}

bool BeanstalkClient::peek_delayed(Job & job)
{
	init();
	BSJ *bsj;
	if (bs_peek_delayed(handle, &bsj) == BS_STATUS_OK)
	{
		job = bsj;
		return true;
	}
	return false;
}

bool BeanstalkClient::peek_buried(Job & job)
{
	init();
	BSJ *bsj;
	if (bs_peek_buried(handle, &bsj) == BS_STATUS_OK)
	{
		job = bsj;
		return true;
	}
	return false;
}

bool BeanstalkClient::kick(int bound)
{
	init();
	return bs_kick(handle, bound) == BS_STATUS_OK;
}

string BeanstalkClient::list_tube_used()
{
	init();
	char *name;
	string tube;
	if (bs_list_tube_used(handle, &name) == BS_STATUS_OK)
	{
		tube.assign(name);
		free(name);
	}
	return tube;
}

info_list_t BeanstalkClient::list_tubes()
{
	init();
	char *yaml, *data;
	info_list_t tubes;
	if (bs_list_tubes(handle, &yaml) == BS_STATUS_OK)
	{
		if ((data = strstr(yaml, "---")))
		{
			stringstream stream(data);
			parselist(stream, tubes);
		}
		free(yaml);
	}
	return tubes;
}

info_list_t BeanstalkClient::list_tubes_watched()
{
	init();
	char *yaml, *data;
	info_list_t tubes;
	if (bs_list_tubes_watched(handle, &yaml) == BS_STATUS_OK)
	{
		if ((data = strstr(yaml, "---")))
		{
			stringstream stream(data);
			parselist(stream, tubes);
		}
		free(yaml);
	}
	return tubes;
}

info_hash_t BeanstalkClient::stats()
{
	init();
	char *yaml, *data;
	info_hash_t stats;
	string key, value;
	if (bs_stats(handle, &yaml) == BS_STATUS_OK)
	{
		if ((data = strstr(yaml, "---")))
		{
			stringstream stream(data);
			parsedict(stream, stats);
		}
		free(yaml);
	}
	return stats;
}

info_hash_t BeanstalkClient::stats_job(int id)
{
	init();
	char *yaml, *data;
	info_hash_t stats;
	string key, value;
	if (bs_stats_job(handle, id, &yaml) == BS_STATUS_OK)
	{
		if ((data = strstr(yaml, "---")))
		{
			stringstream stream(data);
			parsedict(stream, stats);
		}
		free(yaml);
	}
	return stats;
}

info_hash_t BeanstalkClient::stats_tube(string name)
{
	init();
	char *yaml, *data;
	info_hash_t stats;
	string key, value;
	if (bs_stats_tube(handle, (char*) name.c_str(), &yaml) == BS_STATUS_OK)
	{
		if ((data = strstr(yaml, "---")))
		{
			stringstream stream(data);
			parsedict(stream, stats);
		}
		free(yaml);
	}
	return stats;
}

bool BeanstalkClient::ping()
{
	init();
	char *yaml;
	if (bs_list_tubes(handle, &yaml) == BS_STATUS_OK)
	{
		free(yaml);
		return true;
	}
	return false;
}

bool BeanstalkClient::disconnect()
{
	init();
	if (handle > 0 && bs_disconnect(handle) == BS_STATUS_OK)
	{
		handle = -1;
		return true;
	}
	return false;
}

}
