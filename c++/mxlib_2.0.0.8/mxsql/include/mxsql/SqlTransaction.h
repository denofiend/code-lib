/*
 *        Name: SqlTransaction.h
 *
 *  Created on: 2011-2-1
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_SQLTRANSACTION_H__
#define __MXSQL_SQLTRANSACTION_H__

#include <mxsql/SqlException.h>

namespace mxsql
{

class SqlTransaction
{
public:
	SqlTransaction(const SqlTransaction&);
	SqlTransaction& operator =(const SqlTransaction&);
public:
	SqlTransaction(void)
	{
	}

	virtual ~SqlTransaction(void)
	{
	}

	virtual void commit(void) throw (SqlException&) = 0;
	virtual void rollback(void) throw (SqlException&) = 0;
};

} // namespace mxsql

#endif /* __MXSQL_SQLTRANSACTION_H__ */
