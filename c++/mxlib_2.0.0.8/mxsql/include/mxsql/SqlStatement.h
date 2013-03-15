/*
 *        Name: SqlStatement.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_SQLSTATEMENT_H__
#define __MXSQL_SQLSTATEMENT_H__

#include <mxsql/SqlException.h>
#include <mxsql/SqlResultSet.h>

namespace mxsql
{

class SqlConnection;

class SqlStatement
{
private:
	SqlStatement(const SqlStatement&);
	SqlStatement& operator =(const SqlStatement&);
public:
	SqlStatement()
	{
	}

	virtual ~SqlStatement()
	{
	}

	virtual bool execute(const std::string& sql) throw (SqlException&) = 0;

	virtual uint64_t executeUpdate(const std::string& sql)
			throw (SqlException&) = 0;

	virtual SqlResultSet* executeQuery(const std::string& sql)
			throw (SqlException&) = 0;

	virtual void close(void) = 0;

	virtual bool isClosed(void) const = 0;

	virtual SqlResultSet* getResultSet(void) throw (SqlException&) = 0;

	virtual SqlConnection* getConnection(void) const = 0;

	virtual SqlResultSet::enum_type getResultType(void) const = 0;

	virtual void setResultType(SqlResultSet::enum_type resultType) = 0;
};

class SqlPreparedStatement: public SqlStatement
{
private:
	SqlPreparedStatement(const SqlPreparedStatement&);
	SqlPreparedStatement& operator =(const SqlPreparedStatement&);
public:
	SqlPreparedStatement()
	{
	}

	virtual ~SqlPreparedStatement()
	{
	}

	virtual bool execute(void) throw (SqlException&) = 0;

	virtual uint32_t executeUpdate(void) throw (SqlException&) = 0;

	virtual SqlResultSet* executeQuery(void) throw (SqlException&) = 0;

	virtual void setNull(uint32_t index) throw (SqlException&) = 0;

	virtual void
	setShort(uint32_t index, int16_t value) throw (SqlException&) = 0;

	virtual void setUShort(uint32_t index, uint16_t value)
			throw (SqlException&) = 0;

	virtual void
	setInt(uint32_t index, int32_t value) throw (SqlException&) = 0;

	virtual void
	setUInt(uint32_t index, uint32_t value) throw (SqlException&) = 0;

	virtual void
	setLong(uint32_t index, int64_t value) throw (SqlException&) = 0;

	virtual void
	setULong(uint32_t index, uint64_t value) throw (SqlException&) = 0;

	virtual void
	setFloat(uint32_t index, float value) throw (SqlException&) = 0;

	virtual void
	setDouble(uint32_t index, double value) throw (SqlException&) = 0;

	virtual void setString(uint32_t index, const char* value, size_t len)
			throw (SqlException&) = 0;

	virtual void setString(uint32_t index, const std::string& value)
			throw (SqlException&) = 0;

	virtual void setBytes(uint32_t index, const void* value, size_t len)
			throw (SqlException&) = 0;

	virtual void setBytes(uint32_t index, const std::string& value)
			throw (SqlException&) = 0;

	virtual std::istream& setBlob(uint32_t index, std::istream& in)
			throw (SqlException&) = 0;
};

} // namespace mxsql

#endif /* __MXSQL_SQLSTATEMENT_H__ */
