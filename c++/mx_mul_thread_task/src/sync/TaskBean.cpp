/*
 * Queuetask.cpp
 *
 *  Created on: 2013-3-18
 *      Author: DenoFiend
 */

#include "sync/TaskBean.h"
#include "sync/MyLogger.h"
#include <json/json.h>

namespace mx_mul
{

TaskBean::TaskBean(const std::string&data, uint32_t user_id, uint32_t queue_id,
		uint32_t type) :
		data_(data), user_id_(user_id), queue_id_(queue_id), type_(type)
{
}

TaskBean::~TaskBean()
{
}

TaskBean::TaskBean(const TaskBean & src)
{
	this->data_ = src.data_;
	this->user_id_ = src.user_id_;
	this->queue_id_ = src.queue_id_;
	this->type_ = src.type_;
	this->idc_id_ = src.idc_id_;
}

TaskBean & TaskBean::operator =(const TaskBean & other)
{
	if (this != &other)
	{
		this->data_ = other.data_;
		this->user_id_ = other.user_id_;
		this->queue_id_ = other.queue_id_;
		this->type_ = other.type_;
		this->idc_id_ = other.idc_id_;
	}
	return *this;
}

std::string TaskBean::toJsonString()
{
	Json::Value data;

	data["user_id"] = this->user_id_;
	data["from_queue_id"] = this->queue_id_;
	data["from_idc_id"] = this->idc_id_;
	data["type"] = this->type_;

	try
	{
		Json::Reader reader;
		Json::Value jsonData;

		if (!reader.parse(this->data_, jsonData))
		{
			return "";
		}

		if (!jsonData["register_ip"].isNull())
		{
			std::string register_ip = jsonData["register_ip"].asString();

			jsonData["ip"] = register_ip;
		}
		else
		{
			jsonData.removeMember("register_ip");
		}
		if (0 == this->type_)
			jsonData["first_name"] = Json::Value::null;

		jsonData.removeMember("register_ip");

		data["data"] = jsonData;

	} catch (std::exception& e)
	{
		return "";
	}

	Json::FastWriter writer;
	return writer.write(data);
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

std::string TaskBean::getData() const
{
	return data_;
}

uint32_t TaskBean::getIdcId() const
{
	return idc_id_;
}

void TaskBean::setData(std::string data_)
{
	this->data_ = data_;
}

void TaskBean::setIdcId(uint32_t idc_id_)
{
	this->idc_id_ = idc_id_;
}

} /* namespace mx_mul */
