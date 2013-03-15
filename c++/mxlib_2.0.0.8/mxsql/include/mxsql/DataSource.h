/*
 * DataSource.h
 *
 *  Created on: 2011-4-18
 *      Author: zhaojiangang
 */

#ifndef __MYSQL_DATASOURCE_H__
#define __MYSQL_DATASOURCE_H__

#include <memory>
#include <mxsql/DbConfig.h>
#include <mxsql/SqlConnection.h>

namespace mxsql
{

class DataSource
{
public:
	class Config
	{
	public:
		Config(void);
		Config(const Config& src);
		~Config(void);

		Config& operator =(const Config& other);
	public:
		uint32_t idleCount_;

		uint32_t maxCount_;

		uint32_t idleTimeout_;

		std::string heartbeatSql_;

		uint32_t heartbeatInterval_;

		uint32_t waitTimeout_;

		DbConfig dbConfig_;
	};
public:
	virtual ~DataSource(void)
	{
	}

	virtual std::auto_ptr<SqlConnection> getConnection(void)
			throw (SqlException&)= 0;
};

} // namespace mxsql

#endif /* __MYSQL_DATASOURCE_H__ */
