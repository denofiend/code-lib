/*
 *        Name: MysqlTransaction.h
 *
 *  Created on: 2011-2-1
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_MYSQLTRANSACTION_H__
#define __MXSQL_MYSQLTRANSACTION_H__

#include <mxsql/SqlTransaction.h>

namespace mxsql
{

class MysqlConnection;

class MysqlTransaction: public SqlTransaction
{
public:
	MysqlTransaction(MysqlConnection* connection);
	virtual ~MysqlTransaction(void);

	virtual void commit(void) throw (SqlException&);
	virtual void rollback(void) throw (SqlException&);
private:
	MysqlConnection* connection_;

	enum
	{
		BEGIN, COMMIT, ROLLBACK
	} state_;
};

} // namespace mxsql

#endif /* __MXSQL_MYSQLTRANSACTION_H__ */
