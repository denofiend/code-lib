/*
 *        Name: MysqlPreparedResultSet.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_MYSQL_MYSQLPREPAREDRESULTSET_H__
#define __MXSQL_MYSQL_MYSQLPREPAREDRESULTSET_H__

#include <map>
#include <mysql/mysql.h>
#include <mxsql/SqlResultSet.h>

namespace mxsql
{

class MysqlConnection;

class MysqlPreparedResultSet: public SqlResultSet
{
private:
	MysqlPreparedResultSet(const MysqlPreparedResultSet&);
	MysqlPreparedResultSet& operator =(const MysqlPreparedResultSet&);
public:
	MysqlPreparedResultSet(MysqlConnection* connection, MYSQL_STMT* stmt,
			SqlResultSet::enum_type resultType);

	virtual ~MysqlPreparedResultSet(void);

	virtual void close(void);
	virtual bool isClosed(void) const;

	virtual uint32_t getNumFields(void) const;

	virtual bool next(void) throw (SqlException&);

	virtual bool isNull(uint32_t columnIndex) const;
	virtual bool isNull(const std::string& columnName) const
			throw (SqlException&);

	virtual uint32_t findColumn(const std::string& columnName) const;

	virtual int16_t getShort(uint32_t columnIndex) throw (SqlException&);
	virtual int16_t getShort(const std::string& columnName)
			throw (SqlException&);
	virtual int16_t getShortDefault(uint32_t columnIndex, int16_t defaultValue)
			throw (SqlException&);
	virtual int16_t getShortDefault(const std::string& columnName,
			int16_t defaultValue) throw (SqlException&);

	virtual uint16_t getUShort(uint32_t columnIndex) throw (SqlException&);
	virtual uint16_t getUShort(const std::string& columnName)
			throw (SqlException&);
	virtual uint16_t getUShortDefault(uint32_t columnIndex,
			uint16_t defaultValue) throw (SqlException&);
	virtual uint16_t getUShortDefault(const std::string& columnName,
			uint16_t defaultValue) throw (SqlException&);

	virtual int32_t getInt(uint32_t columnIndex) throw (SqlException&);
	virtual int32_t
	getInt(const std::string& columnName) throw (SqlException&);
	virtual int32_t getIntDefault(uint32_t columnIndex, int32_t defaultValue)
			throw (SqlException&);
	virtual int32_t getIntDefault(const std::string& columnName,
			int32_t defaultValue) throw (SqlException&);

	virtual uint32_t getUInt(uint32_t columnIndex) throw (SqlException&);
	virtual uint32_t getUInt(const std::string& columnName)
			throw (SqlException&);
	virtual uint32_t
	getUIntDefault(uint32_t columnIndex, uint32_t defaultValue)
			throw (SqlException&);
	virtual uint32_t getUIntDefault(const std::string& columnName,
			uint32_t defaultValue) throw (SqlException&);

	virtual int64_t getLong(uint32_t columnIndex) throw (SqlException&);
	virtual int64_t getLong(const std::string& columnName)
			throw (SqlException&);
	virtual int64_t getLongDefault(uint32_t columnIndex, int64_t defaultValue)
			throw (SqlException&);
	virtual int64_t getLongDefault(const std::string& columnName,
			int64_t defaultValue) throw (SqlException&);

	virtual uint64_t getULong(uint32_t columnIndex) throw (SqlException&);
	virtual uint64_t getULong(const std::string& columnName)
			throw (SqlException&);
	virtual uint64_t getULongDefault(uint32_t columnIndex,
			uint64_t defaultValue) throw (SqlException&);
	virtual uint64_t getULongDefault(const std::string& columnName,
			uint64_t defaultValue) throw (SqlException&);

	virtual float getFloat(uint32_t columnIndex) throw (SqlException&);
	virtual float getFloat(const std::string& columnName);
	virtual float getFloatDefault(uint32_t columnIndex, float defaultValue)
			throw (SqlException&);
	virtual float getFloatDefault(const std::string& columnName,
			float defaultValue);

	virtual double getDouble(uint32_t columnIndex) throw (SqlException&);
	virtual double getDouble(const std::string& columnName)
			throw (SqlException&);
	virtual double getDoubleDefault(uint32_t columnIndex, double defaultValue)
			throw (SqlException&);
	virtual double getDoubleDefault(const std::string& columnName,
			double defaultValue) throw (SqlException&);

	virtual std::string
	getString(uint32_t columnIndex) throw (SqlException&);
	virtual std::string getString(const std::string& columnName)
			throw (SqlException&);

	virtual std::string getBytes(uint32_t columnIndex) throw (SqlException&);
	virtual std::string getBytes(const std::string& columnName)
			throw (SqlException&);

	virtual std::ostream& getBlob(uint32_t columnIndex, std::ostream& out)
			throw (SqlException&);
	virtual std::ostream& getBlob(const std::string& columnName,
			std::ostream& out) throw (SqlException&);
private:
	uint32_t getColumnIndex(const std::string& columnName) const
			throw (SqlException&);

	void getValue(uint32_t columnIndex, void* value, size_t length,
			enum enum_field_types fieldType, bool isUnsigned)
			throw (SqlException&);
private:
	MysqlConnection* connection_;
	MYSQL_STMT* statement_;
	SqlResultSet::enum_type resultType_;

	uint64_t numRows_;
	uint32_t numFields_;
	uint64_t rowPos_;

	MYSQL_BIND* resultBind_;

	std::map<std::string, uint32_t> columnMap_;
};

} // namespace mxsql

#endif /* __MXSQL_MYSQL_MYSQLPREPAREDRESULTSET_H__ */
