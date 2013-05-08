/*
 * SyncTask.h
 *
 *  Created on: 2013-3-15
 *      Author: DenoFiend
 */

#ifndef SYNCTASK_H_
#define SYNCTASK_H_

#include <mxcore/Runnable.h>
#include <mxsql/DataSource.h>

#include "sync/TaskBean.h"

namespace mx_mul
{

class SyncTask: public mxcore::Runnable
{

public:

	SyncTask(mxsql::DataSource* datasource, const std::string&syncUri,
			int sleepTime, uint32_t idc_id, const std::string&logName);

	virtual ~SyncTask();

	virtual void run(void);

private:

	mxsql::DataSource* datasource_;
	std::string syncUri_;
	int sleepTime_;
	uint32_t idc_id_;
	std::string logName_;

private:
	bool getOneTask(TaskBean &task);

	bool isNickNameDuplicate(const std::string&resJson);

	bool delTask(uint32_t queue_id);

	int responseCode(const std::string & json);

	void delOldRecordsAndQueueTask(const uint32_t & qid, const uint32_t &uid,
			const uint32_t&id);

	void modifyTaskNickname(TaskBean&bean);

	std::string resetData(const std::string &json,
			const std::string&newNickname);

	uint32_t getId(const std::string&json);

};

class MinQidTask: public mxcore::Runnable
{
public:
	MinQidTask(mxsql::DataSource* datasource, const std::string&uri,
			int sleepTime, int idcId, const std::string&logName);
	virtual ~MinQidTask();

	virtual void run(void);
private:

	mxsql::DataSource* datasource_;
	std::string uri_;
	int sleepTime_;
	int idcId_;
	std::string logName_;

private:
	uint32_t getMinQidTask();

	std::string getReqJson(uint32_t qid);

	bool responseOk(const std::string & json);
};

} /* namespace mx_user_api */
#endif /* SYNCTASK_H_ */
