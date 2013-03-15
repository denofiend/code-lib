/*
 * MysqlConnectionFactory.h
 *
 *  Created on: 2011-4-28
 *      Author: zhaojiangang
 */

#ifndef __MXSQL_MYSQLCONNECTIONFACTORY_H__
#define __MXSQL_MYSQLCONNECTIONFACTORY_H__

#include <mxsql/SqlConnectionFactory.h>

namespace mxsql
{

class MysqlConnectionFactory: public SqlConnectionFactory
{
public:
	MysqlConnectionFactory(void);

	virtual ~MysqlConnectionFactory(void);

	virtual SqlConnection* newConnection(const DbConfig& dbConfig)
			throw (SqlException&);
};

} // namespace mxsql

#endif /* __MXSQL_MYSQLCONNECTIONFACTORY_H__ */
