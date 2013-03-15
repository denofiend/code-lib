/*
 *        Name: SqlException.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_SQLEXCEPTION_H__
#define __MXSQL_SQLEXCEPTION_H__

#include <mxcore/Exception.h>

namespace mxsql
{

class SqlException: public mxcore::Exception
{
public:
	SqlException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	SqlException(const SqlException& src);
	virtual ~SqlException(void);
	SqlException& operator =(const SqlException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxsql

#endif /* __MXSQL_SQLEXCEPTION_H__ */
