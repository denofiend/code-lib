/*
 * DataSource.cpp
 *
 *  Created on: 2011-4-18
 *      Author: zhaojiangang
 */

#include <mxsql/DataSource.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

DataSource::Config::Config(void) :
	idleCount_(0), maxCount_(0), idleTimeout_(0), heartbeatInterval_(0),
			waitTimeout_(0)
{
}

DataSource::Config::Config(const Config& src) :
	idleCount_(src.idleCount_), maxCount_(src.maxCount_),
			idleTimeout_(src.idleTimeout_), heartbeatSql_(src.heartbeatSql_),
			heartbeatInterval_(src.heartbeatInterval_),
			waitTimeout_(src.waitTimeout_), dbConfig_(src.dbConfig_)
{
}

DataSource::Config::~Config(void)
{
}

DataSource::Config& DataSource::Config::operator =(const Config& other)
{
	if (this != &other)
	{
		idleCount_ = other.idleCount_;
		maxCount_ = other.maxCount_;
		idleTimeout_ = other.idleTimeout_;
		heartbeatSql_ = other.heartbeatSql_;
		heartbeatInterval_ = other.heartbeatInterval_;
		waitTimeout_ = other.waitTimeout_;
		dbConfig_ = other.dbConfig_;
	}
	return *this;
}

} // namespace mxsql
