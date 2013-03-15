/*
 *        Name: SqlConnection.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_SQLCONNECTION_H__
#define __MXSQL_SQLCONNECTION_H__

#include <mxsql/SqlException.h>
#include <mxsql/SqlStatement.h>
#include <mxsql/SqlTransaction.h>

namespace mxsql
{

typedef enum transaction_isolation
{
	TRANSACTION_NONE = 0,
	TRANSACTION_READ_COMMITTED,
	TRANSACTION_READ_UNCOMMITTED,
	TRANSACTION_REPEATABLE_READ,
	TRANSACTION_SERIALIZABLE
} enum_transaction_isolation;

class SqlConnection
{
private:
	SqlConnection(const SqlConnection&);
	SqlConnection& operator =(const SqlConnection&);
public:
	SqlConnection()
	{
	}

	virtual ~SqlConnection()
	{
	}

	virtual void close(void) = 0;
	virtual bool isClosed(void) const = 0;

	virtual void setCharset(const std::string& charset) throw (SqlException&) = 0;
	virtual std::string getCharset(void) throw (SqlException&) = 0;

	virtual bool getAutoCommit(void) const = 0;
	virtual void setAutoCommit(bool autoCommit) throw (SqlException&)= 0;

	virtual enum_transaction_isolation getTransactionIsolation(void) const = 0;
	virtual void setTransactionIsolation(enum_transaction_isolation level)
			throw (SqlException&) = 0;

	virtual SqlStatement* createStatement(void) throw (SqlException&) = 0;
	virtual SqlPreparedStatement* preparedStatement(const std::string& sql)
			throw (SqlException&) = 0;

	virtual SqlTransaction* beginTransaction(void) throw (SqlException&) = 0;
	virtual uint64_t getInsertId(void) const = 0;
};

} // namespace mxsql

#endif /* __MXSQL_SQLCONNECTION_H__ */
