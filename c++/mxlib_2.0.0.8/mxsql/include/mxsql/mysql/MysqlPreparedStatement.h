/*
 *        Name: MysqlPreparedStatement.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_MYSQL_MYSQLPREPAREDSTATEMENT_H__
#define __MXSQL_MYSQL_MYSQLPREPAREDSTATEMENT_H__

#include <map>
#include <mysql/mysql.h>
#include <mxsql/SqlStatement.h>

namespace mxsql
{

class MysqlConnection;

class MysqlPreparedStatement: public SqlPreparedStatement
{
private:
	MysqlPreparedStatement(const MysqlPreparedStatement&);
	MysqlPreparedStatement& operator =(const MysqlPreparedStatement&);
public:
	MysqlPreparedStatement(MysqlConnection* connection, MYSQL_STMT* statement,
			SqlResultSet::enum_type resultType);

	virtual ~MysqlPreparedStatement(void);

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

	virtual bool execute(void) throw (SqlException&);

	virtual uint32_t executeUpdate(void) throw (SqlException&);

	virtual SqlResultSet* executeQuery(void) throw (SqlException&);

	virtual void setNull(uint32_t index) throw (SqlException&);

	virtual void
	setShort(uint32_t index, int16_t value) throw (SqlException&);

	virtual void setUShort(uint32_t index, uint16_t value)
			throw (SqlException&);

	virtual void
	setInt(uint32_t index, int32_t value) throw (SqlException&);

	virtual void
	setUInt(uint32_t index, uint32_t value) throw (SqlException&);

	virtual void
	setLong(uint32_t index, int64_t value) throw (SqlException&);

	virtual void
	setULong(uint32_t index, uint64_t value) throw (SqlException&);

	virtual void
	setFloat(uint32_t index, float value) throw (SqlException&);

	virtual void
	setDouble(uint32_t index, double value) throw (SqlException&);

	virtual void setString(uint32_t index, const char* value, size_t len)
			throw (SqlException&);

	virtual void setString(uint32_t index, const std::string& value)
			throw (SqlException&);

	virtual void setBytes(uint32_t index, const void* value, size_t len)
			throw (SqlException&);

	virtual void setBytes(uint32_t index, const std::string& value)
			throw (SqlException&);

	virtual std::istream& setBlob(uint32_t index, std::istream& in)
			throw (SqlException&);
private:
	bool isParamSet(uint32_t index) const;

	void setParam(uint32_t index, const void* data, size_t len,
			enum enum_field_types type, bool isUnsigned);

	void bindParams(void) throw (SqlException&);

	bool executeImpl(void) throw (SqlException&);
private:
	MysqlConnection* connection_;
	MYSQL_STMT* statement_;

	uint32_t numParams_;
	MYSQL_BIND* paramsBind_;

	SqlResultSet::enum_type resultType_;

	std::map<uint32_t, uint32_t> nullParams_;
	std::map<uint32_t, std::string> stringParams_;
	std::map<uint32_t, std::istream*> istreamsParams_;
};

} // namespace mxsql

#endif /* __MXSQL_MYSQL_MYSQLPREPAREDSTATEMENT_H__ */
