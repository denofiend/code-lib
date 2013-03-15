/*
 *        Name: MysqlConnection.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_MYSQL_MYSQLCONNECTION_H__
#define __MXSQL_MYSQL_MYSQLCONNECTION_H__

#include <mysql/mysql.h>
#include <mxsql/DbConfig.h>
#include <mxsql/SqlConnection.h>

namespace mxsql
{

class MysqlConnection: public SqlConnection
{
private:
	MysqlConnection(const MysqlConnection&);
	MysqlConnection& operator =(const MysqlConnection&);
public:
	MysqlConnection(void);
	virtual ~MysqlConnection(void);

	void open(const DbConfig& dbConfig) throw (SqlException&);

	virtual void close(void);
	virtual bool isClosed(void) const;

	virtual void setCharset(const std::string& charset) throw (SqlException&);
	virtual std::string getCharset(void) throw (SqlException&);

	virtual bool getAutoCommit(void) const;
	virtual void setAutoCommit(bool autoCommit) throw (SqlException&);

	virtual enum_transaction_isolation getTransactionIsolation(void) const;
	virtual void setTransactionIsolation(enum_transaction_isolation level)
			throw (SqlException&);

	virtual SqlStatement* createStatement(void) throw (SqlException&);
	virtual SqlPreparedStatement* preparedStatement(const std::string& sql)
			throw (SqlException&);

	virtual void commit(void) throw (SqlException&);
	virtual void rollback(void) throw (SqlException&);

	virtual SqlTransaction* beginTransaction(void) throw (SqlException&);

	virtual uint64_t getInsertId(void) const;

	MYSQL* getConnection(void) const;
private:
	MYSQL* connection_;
	bool autoCommit_;
	enum_transaction_isolation txLevel_;
};

} // namespace mxsql

#endif /* __MXSQL_MYSQL_MYSQLCONNECTION_H__ */
