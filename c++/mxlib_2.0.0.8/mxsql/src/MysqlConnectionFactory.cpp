/*
 * MysqlConnectionFactory.cpp
 *
 *  Created on: 2011-4-28
 *      Author: zhaojiangang
 */

#include <mxsql/mysql/MysqlConnection.h>
#include <mxsql/mysql/MysqlConnectionFactory.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

MysqlConnectionFactory::MysqlConnectionFactory()
{
}

MysqlConnectionFactory::~MysqlConnectionFactory()
{
}

SqlConnection * MysqlConnectionFactory::newConnection(const DbConfig& dbConfig)
		throw (SqlException&)
{
	MysqlConnection* connection = new MysqlConnection();

	try
	{
		connection->open(dbConfig);
		return connection;
	} catch (SqlException& e)
	{
		delete connection;
		throw e;
	}
}

} // namespace mxsql
