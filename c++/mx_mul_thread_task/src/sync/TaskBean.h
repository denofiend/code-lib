/*
 * Queuetask.h
 *
 *  Created on: 2013-3-18
 *      Author: DenoFiend
 */

#ifndef TASKBEAN_H
#define TASKBEAN_H

#include <string>
#include <stdint.h>
#include <json/json.h>

namespace mx_mul
{

class TaskBean
{
public:

	TaskBean();
	TaskBean(const std::string&json, uint32_t user_id, uint32_t queue_id,
			uint32_t type);

	virtual ~TaskBean();

	TaskBean(const TaskBean& src);
	TaskBean& operator =(const TaskBean& other);

	std::string toJsonString();
	std::string getJson() const;
	uint32_t getQueueId() const;
	uint32_t getType() const;
	uint32_t getUserId() const;
	void setJson(std::string json_);
	void setQueueId(uint32_t queue_id_);
	void setType(uint32_t type_);
	void setUserId(uint32_t user_id_);

private:
	std::string json_;
	uint32_t user_id_;
	uint32_t queue_id_;
	uint32_t type_;
};

} /* namespace mx_mul */
#endif /* QUEUETASK_H_ */
