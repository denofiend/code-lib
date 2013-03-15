/*
 *        Name: MysqlStatement.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_MYSQLSTATEMENT_H__
#define __MXSQL_MYSQLSTATEMENT_H__

#include <mxsql/SqlStatement.h>
#include <mxsql/SqlResultSet.h>

namespace mxsql
{

class MysqlConnection;

class MysqlStatement: public SqlStatement
{
private:
	MysqlStatement(const MysqlStatement&);
	MysqlStatement& operator=(const MysqlStatement&);
public:
	MysqlStatement(MysqlConnection* connection,
			SqlResultSet::enum_type resultType);

	virtual ~MysqlStatement(void);

	virtual bool execute(const std::string& sql) throw (SqlException&);

	virtual uint64_t executeUpdate(const std::string& sql)
			throw (SqlException&);

	virtual SqlResultSet* executeQuery(const std::string& sql)
			throw (SqlException&);

	virtual void close(void);

	virtual bool isClosed(void) const;

	virtual SqlResultSet* getResultSet(void) throw (SqlException&);

	virtual SqlConnection* getConnection(void) const;

	virtual SqlResultSet::enum_type getResultType(void) const;

	virtual void setResultType(SqlResultSet::enum_type resultType);
private:
	bool isClosed_;
	MysqlConnection* connection_;
	SqlResultSet::enum_type resultType_;
	uint64_t lastUpdateCount_;
};

} // namespace mxsql

#endif /* __MXSQL_MYSQLSTATEMENT_H__ */
