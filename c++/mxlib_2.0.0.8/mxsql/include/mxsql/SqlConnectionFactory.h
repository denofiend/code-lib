/*
 * SqlConnectionFactory.h
 *
 *  Created on: 2011-4-28
 *      Author: zhaojiangang
 */

#ifndef __MXSQL_SQLCONNECTIONFACTORY_H__
#define __MXSQL_SQLCONNECTIONFACTORY_H__

#include <mxsql/DbConfig.h>
#include <mxsql/SqlConnection.h>

namespace mxsql
{

class SqlConnectionFactory
{
public:
	virtual ~SqlConnectionFactory(void)
	{
	}

	virtual SqlConnection
	* newConnection(const DbConfig& dbConfig) = 0;
};

} // namespace mxsql

#endif /* __MXSQL_SQLCONNECTIONFACTORY_H__ */
