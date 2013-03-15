/*
 * DbConfig.cpp
 *
 *  Created on: 2011-4-28
 *      Author: zhaojiangang
 */

#include <mxsql/DbConfig.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

DbConfig::DbConfig(void) :
	port_(0), autoCommit_(true), connectTimeout_(5)
{
}

DbConfig::DbConfig(const DbConfig& src) :
	host_(src.host_), port_(src.port_), username_(src.username_),
			password_(src.password_), dbname_(src.dbname_),
			charset_(src.charset_), autoCommit_(src.autoCommit_),
			connectTimeout_(src.connectTimeout_)
{
}

DbConfig::~DbConfig(void)
{
}

DbConfig& DbConfig::operator =(const DbConfig& other)
{
	if (this != &other)
	{
		host_ = other.host_;
		port_ = other.port_;
		username_ = other.username_;
		password_ = other.password_;
		dbname_ = other.dbname_;
		charset_ = other.charset_;
		autoCommit_ = other.autoCommit_;
		connectTimeout_ = other.connectTimeout_;
	}
	return *this;
}

} // namespace mxsql
