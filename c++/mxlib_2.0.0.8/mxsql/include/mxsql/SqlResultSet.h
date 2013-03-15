/*
 *        Name: SqlResultSet.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXSQL_SQLRESULTSET_H__
#define __MXSQL_SQLRESULTSET_H__

#include <stdint.h>
#include <mxsql/SqlException.h>

namespace mxsql
{

class SqlResultSet
{
private:
	SqlResultSet(const SqlResultSet&);
	SqlResultSet& operator =(const SqlResultSet&);
public:
	typedef enum
	{
		TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE, TYPE_SCROLL_SENSITIVE
	} enum_type;
public:
	SqlResultSet()
	{
	}

	virtual ~SqlResultSet()
	{
	}

	virtual void close(void) = 0;
	virtual bool isClosed(void) const = 0;

	virtual uint32_t getNumFields(void) const = 0;

	virtual bool next(void) throw (SqlException&) = 0;

	virtual bool isNull(uint32_t columnIndex) const =0;
	virtual bool isNull(const std::string& columnName) const
			throw (SqlException&) = 0;

	virtual uint32_t findColumn(const std::string& columnName) const = 0;

	virtual int16_t getShort(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual int16_t getShort(const std::string& columnName)
			throw (SqlException&) = 0;
	virtual int16_t getShortDefault(uint32_t columnIndex, int16_t defaultValue)
			throw (SqlException&) = 0;
	virtual int16_t getShortDefault(const std::string& columnName,
			int16_t defaultValue) throw (SqlException&) = 0;

	virtual uint16_t getUShort(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual uint16_t getUShort(const std::string& columnName)
			throw (SqlException&) = 0;
	virtual uint16_t getUShortDefault(uint32_t columnIndex,
			uint16_t defaultValue) throw (SqlException&) = 0;
	virtual uint16_t getUShortDefault(const std::string& columnName,
			uint16_t defaultValue) throw (SqlException&) = 0;

	virtual int32_t getInt(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual int32_t
	getInt(const std::string& columnName) throw (SqlException&) = 0;
	virtual int32_t getIntDefault(uint32_t columnIndex, int32_t defaultValue)
			throw (SqlException&) = 0;
	virtual int32_t getIntDefault(const std::string& columnName,
			int32_t defaultValue) throw (SqlException&) = 0;

	virtual uint32_t getUInt(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual uint32_t getUInt(const std::string& columnName)
			throw (SqlException&) = 0;
	virtual uint32_t
			getUIntDefault(uint32_t columnIndex, uint32_t defaultValue)
					throw (SqlException&) = 0;
	virtual uint32_t getUIntDefault(const std::string& columnName,
			uint32_t defaultValue) throw (SqlException&) = 0;

	virtual int64_t getLong(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual int64_t getLong(const std::string& columnName)
			throw (SqlException&) = 0;
	virtual int64_t getLongDefault(uint32_t columnIndex, int64_t defaultValue)
			throw (SqlException&) = 0;
	virtual int64_t getLongDefault(const std::string& columnName,
			int64_t defaultValue) throw (SqlException&) = 0;

	virtual uint64_t getULong(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual uint64_t getULong(const std::string& columnName)
			throw (SqlException&) = 0;
	virtual uint64_t getULongDefault(uint32_t columnIndex,
			uint64_t defaultValue) throw (SqlException&) = 0;
	virtual uint64_t getULongDefault(const std::string& columnName,
			uint64_t defaultValue) throw (SqlException&) = 0;

	virtual float getFloat(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual float getFloat(const std::string& columnName) = 0;
	virtual float getFloatDefault(uint32_t columnIndex, float defaultValue)
			throw (SqlException&) = 0;
	virtual float getFloatDefault(const std::string& columnName,
			float defaultValue) = 0;

	virtual double getDouble(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual double getDouble(const std::string& columnName)
			throw (SqlException&) = 0;
	virtual double getDoubleDefault(uint32_t columnIndex, double defaultValue)
			throw (SqlException&) = 0;
	virtual double getDoubleDefault(const std::string& columnName,
			double defaultValue) throw (SqlException&) = 0;

	virtual std::string
			getString(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual std::string getString(const std::string& columnName)
			throw (SqlException&) = 0;

	virtual std::string getBytes(uint32_t columnIndex) throw (SqlException&) = 0;
	virtual std::string getBytes(const std::string& columnName)
			throw (SqlException&) = 0;

	virtual std::ostream& getBlob(uint32_t columnIndex, std::ostream& out)
			throw (SqlException&) = 0;
	virtual std::ostream& getBlob(const std::string& columnName,
			std::ostream& out) throw (SqlException&) = 0;
};

} // namespace mxsql

#endif /* __MXSQL_SQLRESULTSET_H__ */
