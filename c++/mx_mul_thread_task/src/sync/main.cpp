/*
 * main.cpp
 *
 *  Created on: 2012-2-28
 *      Author: DenoFiend
 */

#include <iostream>
#include <mxsql/DataSource.h>
#include <mxcore/IniParser.h>
#include <mxcore/LoggerFactory.h>
#include <mxsql/DbConfig.h>
#include <mxsql/ThreadedDataSource.h>
#include <mxsql/mysql/MysqlConnectionFactory.h>
#include <mxcore/Thread.h>
#include <mxcore/SystemOps.h>
#include <json/json.h>
#include <fstream>
#include <iostream>
#include <list>
#include "sync/UpdateDbTask.h"
#include "mul_task/TaskThread.h"
#include "mul_task/TaskQueue.h"
#include "sync/MyLogger.h"
#include "sync/Util.h"

using namespace std;

#define threadCount 1

#define UserCount 12074458

struct Config
{
	std::string app_;

	mxsql::DataSource::Config dsConfig;

	std::string syncUri_;

	mxcore::LoggerFactory::LoggerConfig logConfig_;

	int syncSleepTime_;

	std::string minIdUri_;
	int minIdSleepTime_;
	int idcId_;

};

static bool getHomeDir(char* argv[], std::string& homeDir)
{
	homeDir = argv[0];
	size_t pos = homeDir.find_last_of('/');
	if (std::string::npos == pos)
	{
		return false;
	}
	homeDir.erase(pos + 1);
	return true;
}

void parserJsonFromFile(vector<Config>&configs, const std::string&fileName)
{

	//File
	std::ifstream in(fileName.c_str());
	std::string json;

	in.seekg(0, std::ios::end);
	json.reserve((unsigned long) in.tellg());
	in.seekg(0, std::ios::beg);

	json.assign((std::istreambuf_iterator<char>(in)),
	std::istreambuf_iterator<char>());

	//std::cout << "json (" << json << std::endl;

	Json::Reader reader;
	Json::Value jsonValue;

	try
	{
		if (!reader.parse(json, jsonValue))
		{
			std::cout << "Json parse error: json(" << json << std::endl;
		}
	} catch (std::exception& e)
	{
		std::cout << "Json parse error: error( " << e.what() << std::endl;

	}

	for (int i = 0; i < jsonValue.size(); ++i)
	{
		Json::Value app = jsonValue[i];
		Config config;

		MxUtil::decode(app, "app", config.app_);

		Json::Value db = app["db"];

		uint32_t autoCommit;
		MxUtil::decode(db, "autoCommit_", autoCommit);
		config.dsConfig.dbConfig_.autoCommit_ = (autoCommit != 0);

		MxUtil::decode(db, "username_", config.dsConfig.dbConfig_.username_);
		MxUtil::decode(db, "password_", config.dsConfig.dbConfig_.password_);
		MxUtil::decode(db, "charset_", config.dsConfig.dbConfig_.charset_);
		MxUtil::decode(db, "connectTimeout_",
				config.dsConfig.dbConfig_.connectTimeout_);
		MxUtil::decode(db, "host_", config.dsConfig.dbConfig_.host_);
		MxUtil::decode(db, "dbname_", config.dsConfig.dbConfig_.dbname_);
		MxUtil::decode(db, "port_", config.dsConfig.dbConfig_.port_);
		MxUtil::decode(db, "heartbeatInterval_",
				config.dsConfig.heartbeatInterval_);
		MxUtil::decode(db, "heartbeatSql_", config.dsConfig.heartbeatSql_);
		MxUtil::decode(db, "idleCount_", config.dsConfig.idleCount_);
		MxUtil::decode(db, "idleTimeout_", config.dsConfig.idleTimeout_);
		MxUtil::decode(db, "waitTimeout_", config.dsConfig.waitTimeout_);
		MxUtil::decode(db, "maxCount_", config.dsConfig.maxCount_);

		Json::Value sync = app["sync"];

		MxUtil::decode(sync, "name_", config.logConfig_.name_);
		MxUtil::decode(sync, "level_", config.logConfig_.level_);
		MxUtil::decode(sync, "bufferSize_", config.logConfig_.bufferSize_);
		MxUtil::decode(sync, "filepath_", config.logConfig_.filepath_);

		MxUtil::decode(sync, "syncUri_", config.syncUri_);
		MxUtil::decode(sync, "syncSleepTime_", config.syncSleepTime_);

		MxUtil::decode(sync, "minIdUri_", config.minIdUri_);
		MxUtil::decode(sync, "minIdSleepTime_", config.minIdSleepTime_);
		MxUtil::decode(sync, "idcId_", config.idcId_);

		configs.push_back(config);

	}

}

int main(int argc, char* argv[])
{

	std::string fileName;

	std::string homeDir;
	getHomeDir(argv, homeDir);

	fileName = homeDir + "../conf/conf.ini";

	std::cout << "fileName = " + fileName << std::endl;

	vector<Config> configs;

	// parse json file config.
	parserJsonFromFile(configs, fileName);

	mxos::daemonInit(argv[0], "", 0);

	mx_mul::TaskQueuePtr taskQueue(new mx_mul::TaskQueue());
	list<mx_mul::TaskThreadPtr> threads;

	for (int i = 0; i < 1; ++i)
	{
		mx_mul::TaskThreadPtr t(new mx_mul::TaskThread(taskQueue));
		threads.push_back(t);
		t->start();
	}

	mxsql::MysqlConnectionFactory factory;

	while (1)
	{

		for (vector<Config>::const_iterator it = configs.begin();
				it != configs.end(); ++it)
				{
			Config conf = *it;

			mxsql::DataSource* dataSource = new mxsql::ThreadedDataSource(
					conf.dsConfig, &factory);

			mxcore::LoggerFactory::getInstance().createLogger(conf.logConfig_);

			mx_mul::SyncTaskPtr task(
					new mx_mul::SyncTask(dataSource, conf.syncUri_,
							conf.syncSleepTime_, conf.idcId_,
							conf.logConfig_.name_));

			taskQueue->push(task);

			mx_mul::MinQidTaskPtr minQidTask(
					new mx_mul::MinQidTask(dataSource, conf.minIdUri_,
							conf.minIdSleepTime_, conf.idcId_,
							conf.logConfig_.name_));

			taskQueue->push(minQidTask);
		}

		mxcore::Thread::sleep(3000);

	}

	for (int i = 0; i < threadCount; ++i)
	{
		mx_mul::SyncTaskPtr task;
		taskQueue->push(task);
	}
	//
	for (list<mx_mul::TaskThreadPtr>::iterator it = threads.begin();
			it != threads.end(); ++it)
			{

		it->get()->join();
	}

	return 0;
}

