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
#include <mxcore/Number.h>

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

int SyncTask::responseCode(const std::string & json)
{
	Json::Reader reader;
	Json::Value jsonValue;

	try
	{
		if (!reader.parse(json, jsonValue))
		{
			logger(logName_).error("Json parse error: json(%s)\n",
					json.c_str());
			return false;
		}
	} catch (std::exception& e)
	{
		logger(logName_).error("Json parse error: json(%s) error(%s)\n",
				json.c_str(), e.what());
		return false;
	}

	uint32_t code;
	if (decode(jsonValue, "code", code))
	{
		return code;
	}
	else
	{
		return -1;
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
	int code = responseCode(resJson);

	logger(logName_).debug("user-api-center.maxthon.com type(%d), app(%s)\n", task.getType(), logName_.c_str());

	if ((201 == code || 200 == code) && 1 == task.getType() && "user_api" == logName_)
	{
		delOldRecordsAndQueueTask(task.getQueueId(), task.getUserId(),
				getId(task.getData()));
	}
	else if (200 == code || 409 == code || 201 == code)
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
			task.setAppName(logName_);

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

uint32_t SyncTask::getId(const std::string & json)
{
	Json::Reader reader;
	Json::Value jsonValue;

	try
	{
		if (!reader.parse(json, jsonValue))
		{
			logger(logName_).error("Json parse error: json(%s)\n",
					json.c_str());
			return false;
		}
	} catch (std::exception& e)
	{
		logger(logName_).error("Json parse error: json(%s) error(%s)\n",
				json.c_str(), e.what());
		return false;
	}

	uint32_t id;
	if (decode(jsonValue, "id", id))
	{
		return id;
	}
	else
	{
		return 0;
	}
}

void SyncTask::delOldRecordsAndQueueTask(const uint32_t & qid,
		const uint32_t &uid, const uint32_t&id)
{
	logger(logName_).trace(">>> SyncTask::delTask\n");

	std::auto_ptr<mxsql::SqlConnection> connection(
			datasource_->getConnection());

	std::auto_ptr<mxsql::SqlTransaction> trans(
			connection->beginTransaction());
	try
	{
		int ret = false;

		// delete transaction_table
		{
			const std::string sql =
					"delete from `roll_transaction` where `queue_id` = ?";

			std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
					connection->preparedStatement(sql));
			stmt->setUInt(1, qid);

			ret = stmt->executeUpdate();
		}

		std::string account, email, mobile, country_code, nickname;
		unsigned long long int id_a = 0, id_e = 0, id_m = 0, id_c = 0, id_n = 0;

		{
			const std::string sql =
					"select `account`, `email`, `mobile`, `country_code`, `nickname`, `id` from `base_user_info` where `user_id` = ? and `id` < ? for update";

			std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
					connection->preparedStatement(sql));

			stmt->setUInt(1, uid);
			stmt->setUInt(2, id);

			std::auto_ptr<mxsql::SqlResultSet> rs(stmt->executeQuery());

			while (rs->next())
			{
				unsigned long long id_now = rs->getULong(6);
				if (!rs->isNull(1))
				{
					std::string account_temp = rs->getString(1);
					if (id_a < id_now)
					{
						id_a = id_now;
						account = account_temp;
					}
					
				}
				if (!rs->isNull(2))
				{
					std::string email_temp = rs->getString(2);
					if (id_e < id_now)
					{
						id_e = id_now;
						email = email_temp;
					}
				}
				if (!rs->isNull(3))
				{
					std::string mobile_temp = rs->getString(3);
					if (id_m < id_now)
					{
						id_m = id_now;
						mobile = mobile_temp;
					}
				}
				if (!rs->isNull(4))
				{
					std::string country_code_temp = rs->getString(4);
					if (id_c < id_now)
					{
						id_c = id_now;
						country_code = country_code_temp;
					}
				}
				if (!rs->isNull(5))
				{
					std::string nickname_temp = rs->getString(5);
					if (id_n < id_now)
					{
						id_n = id_now;
						nickname = nickname_temp;
					}
				}
			}
		}

		logger(logName_).debug("after select account(%s) email(%s), mobile(%s), country_code(%s), nickname(%s)\n", account.c_str(), email.c_str(), mobile.c_str(), country_code.c_str(), nickname.c_str());

		{
			const std::string sql =
					"delete from `base_user_info` where `user_id` = ? and `id` < ?";
			std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
					connection->preparedStatement(sql));
			stmt->setUInt(1, uid);
			stmt->setUInt(2, id);

			ret = stmt->executeUpdate();
		}

		{
			const std::string sql =
					"update `base_user_info` set `account` = ifnull(`account`, ?), "
							"`email` = ifnull(`email`, ?), "
							"`mobile` = ifnull(`mobile`, ?), "
							"`country_code` = ifnull(`country_code`, ?), "
							"`nickname` = ifnull(`nickname`, ?) where `user_id` = ?  and `id` = ?;";

			logger(logName_).debug("mysql sql(%s)\n", sql.c_str());

			std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
					connection->preparedStatement(sql));

			if (account.empty())
			{
				stmt->setNull(1);
			}
			else
			{
				stmt->setString(1, account);
			}

			if (email.empty())
			{
				stmt->setNull(2);
			}
			else
			{
				stmt->setString(2, email);
			}

			if (mobile.empty())
			{
				stmt->setNull(3);
			}
			else
			{
				stmt->setString(3, mobile);
			}

			if (country_code.empty())
			{
				stmt->setNull(4);
			}
			else
			{
				stmt->setUInt(4, mxcore::UInteger::fromString(country_code));
			}

			if (nickname.empty())
			{
				stmt->setNull(5);
			}
			else
			{
				stmt->setString(5, nickname);
			}

			stmt->setUInt(6, uid);
			stmt->setUInt(7, id);

			ret = stmt->executeUpdate();
		}

		trans->commit();

	} catch (mxsql::SqlException&e)
	{
		trans->rollback();
		logger(logName_).error("mysql error: code(%d) message(%s)\n",
				e.getErrorCode(), e.getMessage().c_str());
	}
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

	logger(logName_).info(">>> MinQidTask center response(%s)\n",
			resJson.c_str());

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

			return false;

		}

	} catch (std::exception& e)
	{
		logger(logName_).error("Json parse error: json(%s) error(%s)\n",
				json.c_str(), e.what());

		return false;
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
