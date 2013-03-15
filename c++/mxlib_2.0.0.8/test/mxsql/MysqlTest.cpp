/*
 *        Name: MysqlTest.cpp
 *
 *  Created on: 2011-2-1
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include "MysqlTest.h"
#include <fstream>
#include <iostream>
#include <mxcore/Assert.h>
#include <mxsql/mysql/MysqlConnection.h>
#include <mxcore/MemRecord.h>

using namespace std;

namespace mxsql
{

MysqlTest::MysqlTest()
{
}

MysqlTest::~MysqlTest()
{
}

void MysqlTest::init()
{
}

void test_statement(MysqlConnection& connection)
{
	std::auto_ptr<SqlStatement> stmt(connection.createStatement());

	std::auto_ptr<SqlResultSet> rs(
			stmt->executeQuery("SELECT * FROM favorite.test_table"));

	while (rs->next())
	{
		cout << "--------------------------------------------" << endl;
		cout << "user_id:" << rs->getString("user_id") << endl;
		cout << "user_name:" << rs->getString("user_name") << endl;
		cout << "--------------------------------------------" << endl;
	}
}

void test_pp_statement(MysqlConnection& connection)
{
	std::auto_ptr<SqlPreparedStatement> stmt(
			connection.preparedStatement(
					"SELECT * FROM favorite.test_table where user_id=?"));
	stmt->setLong(1, 5430131);

	std::auto_ptr<SqlResultSet> rs(stmt->executeQuery());

	while (rs->next())
	{
		cout << "--------------------------------------------" << endl;
		cout << "user_id:" << rs->getUInt("user_id") << endl;
		cout << "user_name:" << rs->getString("user_name") << endl;
		cout << "--------------------------------------------" << endl;
	}
}

void test_insert(MysqlConnection& connection)
{
	for (int i = 0; i < 1; i++)
	{
		std::string
				inser_sql(
						"INSERT INTO favorite.test_table(user_id, user_name) VALUES (?,?)");
		std::auto_ptr<SqlPreparedStatement> stmt(
				connection.preparedStatement(inser_sql));

		std::ifstream in("main.cpp");

		stmt->setLong(1, 540131 + i);
		stmt->setBlob(2, in);
		stmt->executeUpdate();
	}
}

void MysqlTest::perform(void)
{
	MysqlConnection connection;

	std::cout << "MysqlTest::perform ..." << std::endl;
	try
	{
		mxsql::DbConfig dbConfig;
		dbConfig.host_ = "localhost";
		dbConfig.charset_ = "utf8";
		dbConfig.port_ = 3306;
		dbConfig.username_="root";
		dbConfig.dbname_ = "favorite";
		dbConfig.autoCommit_ = false;
		connection.open(dbConfig);

		std::auto_ptr<SqlTransaction> tx(connection.beginTransaction());
		std::auto_ptr<SqlStatement> stmt(connection.createStatement());
		stmt->execute("delete from favorite.test_table");
		test_insert(connection);
		test_statement(connection);
		test_pp_statement(connection);
		tx->commit();
	} catch (SqlException& e)
	{
		cout << e.getErrorCode() << ":" << e.getMessage() << ":"
				<< e.getFilename() << ":" << e.getLineNumber() << endl;
		MX_ASSERT(0);
	}
}

void MysqlTest::cleanup(void)
{
}

} // namespace mxsql
