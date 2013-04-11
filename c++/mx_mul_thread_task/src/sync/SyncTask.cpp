/*
 * SyncTask.cpp
 *
 *  Created on: 2013-3-15
 *      Author: DenoFiend
 */

#include "sync/SyncTask.h"
#include "sync/HttpsClient.h"
#include "sync/MyLogger.h"
#include <mxcore/Thread.h>

namespace mx_mul
{

static bool decode(const Json::Value& value, const char* key, uint32_t& dst)
{
	if (value[key].isNull() || !value.isObject())
	{
		//E_JSON_DECODE(std::string("Invalid ") + key + " Parent");
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::uintValue))
	{
		//E_JSON_DECODE(std::string("Invalid ") + key);
		return false;
	}

	dst = subValue.asUInt();
	return true;
}

bool SyncTask::responseOk(const std::string & json)
{
	Json::Reader reader;
	Json::Value jsonValue;

	try
	{
		if (!reader.parse(json, jsonValue))
		{
			logger(logName_).error("Json parse error: json(%s)\n",
					json.c_str());

		}
	} catch (std::exception& e)
	{
		logger(logName_).error("Json parse error: json(%s) error(%s)\n",
				json.c_str(), e.what());
	}

	uint32_t code;
	if (decode(jsonValue, "code", code))
	{
		return (200 == code || 409 == code);
	}
	else
	{
		return false;
	}
}

SyncTask::SyncTask(mxsql::DataSource *datasource, const std::string&syncUri,
		int sleepTime, uint32_t idc_id, const std::string&logName) :
		datasource_(datasource), syncUri_(syncUri), sleepTime_(sleepTime), idc_id_(
				idc_id), logName_(logName)
{

}

SyncTask::~SyncTask()
{

}

void SyncTask::run(void)
{

	logger(logName_).trace(">>> SyncTask::run\n");

	// get one task from queue_table.
	TaskBean task;
	if (!getOneTask(task))
	{

		logger(logName_).info("not found sync task\n");
		mxcore::Thread::sleep(sleepTime_);

		return;
	}

	// sync to user center service of maxthon.
	HttpsClient client;
	std::string json;

	logger(logName_).debug("syncUri(%s)\n", syncUri_.c_str());

	std::string resJson = client.httpPost(syncUri_, task.toJsonString(),
			"application/json");

	logger(logName_).info("user-api-center.maxthon.com response(%s)\n",
			resJson.c_str());

	// if success. del the task.
	if (responseOk(resJson))
	{
		if (delTask(task.getQueueId()))
		{
			logger(logName_).info("Del succes\n");
		}
		else
		{
			logger(logName_).info("Del failed \n");
		}
	}
	else
	{

		logger(logName_).error("center service response error\n");
	}

}

bool SyncTask::getOneTask(TaskBean& task)
{
	logger(logName_).trace(">>> SyncTask::getOneTask\n");
	bool f = false;

	try
	{
		std::string sql =
				"select `json`,`queue_id`,`type`,`user_id` from `roll_transaction` order by `queue_id` limit 1";

		std::auto_ptr<mxsql::SqlConnection> connection(
				datasource_->getConnection());

		std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
				connection->preparedStatement(sql));

		std::auto_ptr<mxsql::SqlResultSet> rs(stmt->executeQuery());

		if (rs->next())
		{
			task.setData(rs->getString(1));
			task.setQueueId(rs->getUInt(2));
			task.setType(rs->getUInt(3));
			task.setUserId(rs->getUInt(4));
			task.setIdcId(idc_id_);

			f = true;
		}

	} catch (mxsql::SqlException & e)
	{
		logger(logName_).error("Mysql Error: code(%d), message(%s)\n",
				e.getErrorCode(), e.getMessage().c_str());
	}

	logger(logName_).info("SyncTask::getOneTask success (%s)\n",
			task.toJsonString().c_str());

	return f;
}

bool SyncTask::delTask(uint32_t queue_id)
{

	logger(logName_).trace(">>> SyncTask::delTask\n");

	std::auto_ptr<mxsql::SqlConnection> connection(
			datasource_->getConnection());

	std::auto_ptr<mxsql::SqlTransaction> trans(connection->beginTransaction());
	int ret = false;

	// delete transaction_table
	{
		const std::string sql =
				"delete from `roll_transaction` where `queue_id` = ?";

		std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
				connection->preparedStatement(sql));
		stmt->setUInt(1, queue_id);

		ret = stmt->executeUpdate();
	}

	trans->commit();

	return (0 != ret);
}

}

mx_mul::MinQidTask::MinQidTask(mxsql::DataSource* datasource,
		const std::string&uri, int sleepTime, int idcId,
		const std::string&logName) :
		datasource_(datasource), uri_(uri), sleepTime_(sleepTime), idcId_(
				idcId), logName_(logName)
{
}

mx_mul::MinQidTask::~MinQidTask()
{
}

std::string mx_mul::MinQidTask::getReqJson(uint32_t qid)
{
	Json::Value data;

	data["from_idc_id"] = this->idcId_;
	data["min_queue_id"] = qid;

	Json::FastWriter writer;
	std::string res = writer.write(data);

	logger(logName_).info("MinQidTask::getReqJson (%s)\n", res.c_str());

	return res;
}
void mx_mul::MinQidTask::run(void)
{
	logger(logName_).trace(">>> MinQidTask::run\n");

	//get min id task in qu
	uint32_t qid = getMinQidTask();

	// send to center service.
	HttpsClient client;
	std::string resJson = client.httpPost(uri_, getReqJson(qid),
			"application/json");

	if (responseOk(resJson))
	{
		logger(logName_).info("send succed\n");
	}
	else
	{
		logger(logName_).info("send failed\n");
	}
	mxcore::Thread::sleep(sleepTime_);
}

uint32_t mx_mul::MinQidTask::getMinQidTask()
{
	logger(logName_).trace(">>> MinQidTask::getMinQidTask\n");

	uint32_t qid;

	try
	{
		std::string sql = "select min(`queue_id`) from `roll_transaction`";

		std::auto_ptr<mxsql::SqlConnection> connection(
				datasource_->getConnection());

		std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
				connection->preparedStatement(sql));

		std::auto_ptr<mxsql::SqlResultSet> rs(stmt->executeQuery());

		if (rs->next())
		{
			qid = rs->getUInt(1);
		}
	} catch (mxsql::SqlException & e)
	{
		logger(logName_).error("Mysql Error: code(%d), message(%s)\n",
				e.getErrorCode(), e.getMessage().c_str());
	}

	logger(logName_).info("MinQidTask::getMinQidTask success qid(%d)\n", qid);

	return qid - 1;
}

bool mx_mul::MinQidTask::responseOk(const std::string & json)
{
	Json::Reader reader;
	Json::Value jsonValue;

	try
	{
		if (!reader.parse(json, jsonValue))
		{
			logger(logName_).error("Json parse error: json(%s)\n",
					json.c_str());

		}
	} catch (std::exception& e)
	{
		logger(logName_).error("Json parse error: json(%s) error(%s)\n",
				json.c_str(), e.what());
	}

	uint32_t code;
	if (decode(jsonValue, "code", code))
	{
		return (200 == code || 409 == code);
	}
	else
	{
		return false;
	}
}
