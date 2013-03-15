/*
 * DataSourceTest.cpp
 *
 *  Created on: 2011-4-28
 *      Author: zhaojiangang
 */

#include <fstream>
#include <iostream>
#include "DataSourceTest.h"
#include <mxcore/Thread.h>
#include <mxsql/DataSource.h>
#include <mxsql/ThreadedDataSource.h>
#include <mxsql/mysql/MysqlConnectionFactory.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

DataSourceTest::DataSourceTest()
{
}

DataSourceTest::~DataSourceTest()
{
}

void DataSourceTest::init(void)
{
}

void test_insert(SqlConnection& connection)
{
	for (int i = 0; i < 1; i++)
	{
		std::string inser_sql(
				"INSERT INTO favorite.test_table(user_name) VALUES (?)");
		std::auto_ptr<SqlPreparedStatement> stmt(
				connection.preparedStatement(inser_sql));

		std::ifstream in("test/main.cpp");

		stmt->setBlob(1, in);
		stmt->executeUpdate();

		std::cout << "Insert Id: " << connection.getInsertId() << std::endl;
	}
}

class ProcessThread: public mxcore::Thread
{
public:
	ProcessThread(DataSource* dataSource) :
		mxcore::Thread(NULL), dataSource_(dataSource)
	{
	}

	virtual void run(void)
	{
		try
		{
			std::cout << "before" << std::endl;
			std::auto_ptr<SqlConnection> connection =
					dataSource_->getConnection();
			std::cout << "after" << std::endl;
			test_insert(*connection);
			//		test_insert(*connection);
			//		transaction->commit();
		} catch (SqlException& e)
		{
			std::cout << e.getMessage() << std::endl;
		}
	}
private:
	DataSource* dataSource_;
};

void DataSourceTest::perform(void)
{
	MysqlConnectionFactory factory;
	DataSource::Config config;

	config.heartbeatSql_ = "";
	config.heartbeatInterval_ = 10000;
	config.idleCount_ = 1;
	config.maxCount_ = 1;
	config.waitTimeout_ = 20000;
	config.idleTimeout_ = 2000;

	config.dbConfig_.host_ = "localhost";
	config.dbConfig_.port_ = 3306;
	config.dbConfig_.username_ = "root";
	config.dbConfig_.password_ = "";
	config.dbConfig_.dbname_ = "favorite";
	config.dbConfig_.autoCommit_ = true;

	ThreadedDataSource dataSource(config, &factory);

	std::auto_ptr<SqlConnection> connection = dataSource.getConnection();
	ProcessThread t(&dataSource);
	t.start();
	//	std::auto_ptr<SqlTransaction> transaction(connection->beginTransaction());
	try
	{
		std::auto_ptr<SqlConnection> connection1 = dataSource.getConnection();
		test_insert(*connection);
		connection.reset(NULL);
		std::cout << "release" << std::endl;
		//		test_insert(*connection);
		//		transaction->commit();
	} catch (SqlException& e)
	{
		connection.reset(NULL);
		std::cout << e.getMessage() << std::endl;
		std::cout << "release1" << std::endl;
	}
	t.join();
}

void DataSourceTest::cleanup(void)
{
}

}
