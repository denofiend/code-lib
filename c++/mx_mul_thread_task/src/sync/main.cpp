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
#include <list>
#include "sync/UpdateDbTask.h"
#include "mul_task//TaskThread.h"
#include "mul_task//TaskQueue.h"
#include "sync/MyLogger.h"

using namespace std;

#define threadCount 2

#define UserCount 12074458

struct Config
{
	mxsql::DataSource::Config dsConfig;

	std::string syncUri_;

	mxcore::LoggerFactory::LoggerConfig logConfig_;

	int syncSleepTime_;

	std::string minIdUri_;
	int minIdSleepTime_;
	int idcId_;

};

void parseConfig(const mxcore::IniParser& parser, Config&conf,
		const std::string& db, const std::string&sync)
{

	int autoCommit;

	parser.getValue(db, "autoCommit_", autoCommit);
	conf.dsConfig.dbConfig_.autoCommit_ = (autoCommit != 0);

	parser.getValue(db, "username_", conf.dsConfig.dbConfig_.username_);
	parser.getValue(db, "password_", conf.dsConfig.dbConfig_.password_);
	parser.getValue(db, "charset_", conf.dsConfig.dbConfig_.charset_);
	parser.getValue(db, "connectTimeout_",
			conf.dsConfig.dbConfig_.connectTimeout_);
	parser.getValue(db, "host_", conf.dsConfig.dbConfig_.host_);
	parser.getValue(db, "dbname_", conf.dsConfig.dbConfig_.dbname_);
	parser.getValue(db, "port_", conf.dsConfig.dbConfig_.port_);
	parser.getValue(db, "heartbeatInterval_", conf.dsConfig.heartbeatInterval_);
	parser.getValue(db, "heartbeatSql_", conf.dsConfig.heartbeatSql_);
	parser.getValue(db, "idleCount_", conf.dsConfig.idleCount_);
	parser.getValue(db, "idleTimeout_", conf.dsConfig.idleTimeout_);
	parser.getValue(db, "waitTimeout_", conf.dsConfig.waitTimeout_);
	parser.getValue(db, "maxCount_", conf.dsConfig.maxCount_);

	parser.getValue(sync, "name_", conf.logConfig_.name_);
	parser.getValue(sync, "level_", conf.logConfig_.level_);
	parser.getValue(sync, "bufferSize_", conf.logConfig_.bufferSize_);
	parser.getValue(sync, "filepath_", conf.logConfig_.filepath_);

	parser.getValue(sync, "syncUri_", conf.syncUri_);
	parser.getValue(sync, "syncSleepTime_", conf.syncSleepTime_);

	parser.getValue(sync, "minIdUri_", conf.minIdUri_);
	parser.getValue(sync, "minIdSleepTime_", conf.minIdSleepTime_);
	parser.getValue(sync, "idcId_", conf.idcId_);
}

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

int main(int argc, char* argv[])
{

	mxos::daemonInit(argv[0], "", 0);

	std::string fileName;

	std::string homeDir;
	getHomeDir(argv, homeDir);

	fileName = homeDir + "../conf/conf.ini";

	std::cout << "fileName = " + fileName << std::endl;

	Config userApiConfig;
	mxcore::IniParser parser;
	parser.parseFile(fileName);
	parseConfig(parser, userApiConfig, "user_api_db", "user_api_sync");

	mxsql::MysqlConnectionFactory factory;

	mxsql::DataSource* dataSource = new mxsql::ThreadedDataSource(
			userApiConfig.dsConfig, &factory);

	mxcore::LoggerFactory::getInstance().createLogger(userApiConfig.logConfig_);
	mx_mul::TaskQueuePtr taskQueue(new mx_mul::TaskQueue());
	list<mx_mul::TaskThreadPtr> threads;

	for (int i = 0; i < threadCount; ++i)
	{
		mx_mul::TaskThreadPtr t(new mx_mul::TaskThread(taskQueue));
		threads.push_back(t);
		t->start();
	}

	while (1)
	{
		logger().info("Begin sync to center\n");
//

		mx_mul::SyncTaskPtr task(
				new mx_mul::SyncTask(dataSource, userApiConfig.syncUri_,
						userApiConfig.syncSleepTime_));

		taskQueue->push(task);

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

