/*
 * main.cpp
 *
 *  Created on: 2012-2-28
 *      Author: DenoFiend
 */

#include <iostream>
#include <mxsql/DataSource.h>
#include <mxsql/DbConfig.h>
#include <mxsql/ThreadedDataSource.h>
#include <mxsql/mysql/MysqlConnectionFactory.h>
#include <list>
#include "UpdateDbTask.h"
#include "TaskThread.h"
#include "TaskQueue.h"

using namespace std;

#define threadCount 100
#define UserCount 12074458

int main()
{
	mxsql::DataSource::Config dsConfig;
	dsConfig.dbConfig_.autoCommit_ = true;
	dsConfig.dbConfig_.username_ = "test";
	dsConfig.dbConfig_.password_ = "maxthon3_test";
	dsConfig.dbConfig_.charset_ = "utf8";
	dsConfig.dbConfig_.connectTimeout_ = 1000;
	dsConfig.dbConfig_.host_ = "10.0.8.65";
	dsConfig.dbConfig_.dbname_ = "test_mx_user_cn";
	dsConfig.dbConfig_.port_ = 3306;

	dsConfig.heartbeatInterval_ = 20000;
	dsConfig.heartbeatSql_ = "select `user_id` from base_user_info limit 1";
	dsConfig.idleCount_ = 5;
	dsConfig.idleTimeout_ = 20;
	dsConfig.waitTimeout_ = 1000;
	dsConfig.maxCount_ = threadCount + threadCount;

	mxsql::MysqlConnectionFactory factory;

	mxsql::DataSource* dataSource = new mxsql::ThreadedDataSource(dsConfig,
			&factory);

	mx_mul::TaskQueuePtr taskQueue(new mx_mul::TaskQueue());
	list<mx_mul::TaskThreadPtr> threads;

	for (int i = 0; i < threadCount; ++i)
	{
		mx_mul::TaskThreadPtr t(new mx_mul::TaskThread(taskQueue));
		threads.push_back(t);
		t->start();
	}

	std::string nickname = "";

	for (int userId = 1; userId <= UserCount; ++userId)
	{
		mx_mul::UpdateDbTaskPtr task(
				new mx_mul::UpdateDbTask(dataSource, userId, nickname));

		taskQueue->push(task);
	}

	for (int i = 0; i < threadCount; ++i)
	{
		mx_mul::UpdateDbTaskPtr task;
		taskQueue->push(task);
	}

	for (list<mx_mul::TaskThreadPtr>::iterator it = threads.begin();
			it != threads.end(); ++it)
			{

		it->get()->join();
	}
	return 0;
}

