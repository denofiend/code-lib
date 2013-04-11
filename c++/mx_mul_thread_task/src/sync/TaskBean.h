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
	TaskBean(const std::string&data, uint32_t user_id, uint32_t queue_id,
			uint32_t type, const std::string& appName);

	virtual ~TaskBean();

	TaskBean(const TaskBean& src);
	TaskBean& operator =(const TaskBean& other);

	std::string toJsonString();
	uint32_t getQueueId() const;
	uint32_t getType() const;
	uint32_t getUserId() const;
	void setQueueId(uint32_t queue_id_);
	void setType(uint32_t type_);
	void setUserId(uint32_t user_id_);
	std::string getData() const;
	uint32_t getIdcId() const;
	void setData(std::string data_);
	void setIdcId(uint32_t idc_id_);
	std::string getAppName() const;
	void setAppName(std::string appName_);

private:
	std::string data_;
	uint32_t user_id_;
	uint32_t queue_id_;
	uint32_t type_;
	uint32_t idc_id_;
	std::string appName_;
};

} /* namespace mx_mul */
#endif /* QUEUETASK_H_ */
