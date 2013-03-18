/*
 * Queuetask.cpp
 *
 *  Created on: 2013-3-18
 *      Author: DenoFiend
 */

#include "sync/TaskBean.h"
#include <json/json.h>

namespace mx_mul
{

TaskBean::TaskBean(const std::string&json, uint32_t user_id, uint32_t queue_id,
		uint32_t type) :
		json_(json), user_id_(user_id), queue_id_(queue_id), type_(type)
{
}

TaskBean::~TaskBean()
{
}

TaskBean::TaskBean(const TaskBean & src)
{
	this->json_ = src.json_;
	this->user_id_ = src.user_id_;
	this->queue_id_ = src.queue_id_;
	this->type_ = src.type_;
}

TaskBean & TaskBean::operator =(const TaskBean & other)
{
	if (this != &other)
	{
		this->json_ = other.json_;
		this->user_id_ = other.user_id_;
		this->queue_id_ = other.queue_id_;
		this->type_ = other.type_;
	}
	return *this;
}

std::string TaskBean::toJsonString()
{
	Json::Value data;

	data["user_id"] = this->user_id_;
	data["queue_id"] = this->queue_id_;
	data["type"] = this->type_;
	data["json"] = this->json_;

	Json::FastWriter writer;
	return writer.write(data);
}

std::string TaskBean::getJson() const
{
	return json_;
}

uint32_t TaskBean::getQueueId() const
{
	return queue_id_;
}

uint32_t TaskBean::getType() const
{
	return type_;
}

uint32_t TaskBean::getUserId() const
{
	return user_id_;
}

void TaskBean::setJson(std::string json_)
{
	this->json_ = json_;
}

void TaskBean::setQueueId(uint32_t queue_id_)
{
	this->queue_id_ = queue_id_;
}

void TaskBean::setType(uint32_t type_)
{
	this->type_ = type_;
}

TaskBean::TaskBean()
{
}

void TaskBean::setUserId(uint32_t user_id_)
{
	this->user_id_ = user_id_;
}

} /* namespace mx_mul */
