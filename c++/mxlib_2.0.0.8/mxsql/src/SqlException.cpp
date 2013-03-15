/*
 *        Name: SqlException.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxsql/SqlException.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

SqlException::SqlException(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	Exception(message, errorCode, filename, line)
{
}

SqlException::SqlException(const SqlException& src) :
	Exception(src)
{
}

SqlException::~SqlException(void)
{
}

SqlException& SqlException::operator =(const SqlException& other)
{
	Exception::operator=(other);
	return *this;
}

const std::string& SqlException::name(void) const
{
	static const std::string sName("mxsql::SqlException");
	return sName;
}

} // namespace mxsql
