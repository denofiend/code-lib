/*
 * DbConfig.h
 *
 *  Created on: 2011-4-28
 *      Author: zhaojiangang
 */

#ifndef __MXSQL_DBCONFIG_H__
#define __MXSQL_DBCONFIG_H__

#include <string>
#include <stdint.h>

namespace mxsql
{

class DbConfig
{
public:
	DbConfig(void);
	DbConfig(const DbConfig& src);
	~DbConfig(void);

	DbConfig& operator =(const DbConfig& other);
public:
	std::string host_;

	uint32_t port_;

	std::string username_;

	std::string password_;

	std::string dbname_;

	std::string charset_;

	bool autoCommit_;

	uint32_t connectTimeout_;
};

} // namespace mxsql

#endif /* __MXSQL_DBCONFIG_H__ */
