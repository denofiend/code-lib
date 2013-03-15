/*
 *        Name: MysqlResultSet.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Real.h>
#include <mxcore/Number.h>
#include <mxsql/mysql/MysqlResultSet.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

MysqlResultSet::MysqlResultSet(MYSQL_RES* result,
		SqlResultSet::enum_type resultType) :
	row_(NULL), result_(result), resultType_(resultType)
{
	numRows_ = ::mysql_num_rows(result_);
	numFields_ = ::mysql_num_fields(result_);
	rowPos_ = 0;

	MYSQL_FIELD* fields = ::mysql_fetch_fields(result_);

	for (uint32_t i = 0; i < numFields_; i++)
	{
		columnMap_[fields[i].name] = i + 1;
	}
}

MysqlResultSet::~MysqlResultSet(void)
{
	close();
}

void MysqlResultSet::close(void)
{
	if (!isClosed())
	{
		::mysql_free_result(result_);
		result_ = NULL;
		row_ = NULL;
	}
}

bool MysqlResultSet::isClosed(void) const
{
	return NULL == result_;
}

uint32_t MysqlResultSet::getNumFields(void) const
{
	MX_ASSERT(!isClosed());
	return numFields_;
}

bool MysqlResultSet::next(void) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	if (rowPos_ >= numRows_)
	{
		row_ = NULL;
		fieldLengths_ = NULL;
		return false;
	}

	rowPos_++;
	row_ = ::mysql_fetch_row(result_);
	fieldLengths_ = ::mysql_fetch_lengths(result_);

	return true;
}

bool MysqlResultSet::isNull(uint32_t columnIndex) const
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return NULL == row_[columnIndex - 1];
}

bool MysqlResultSet::isNull(const std::string& columnName) const
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return isNull(getColumnIndex(columnName));
}

uint32_t MysqlResultSet::findColumn(const std::string& columnName) const
{
	MX_ASSERT(!isClosed());

	std::map<std::string, uint32_t>::const_iterator it = columnMap_.find(
			columnName);

	if (it == columnMap_.end())
	{
		return 0;
	}

	return it->second;
}

int16_t MysqlResultSet::getShort(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return static_cast<int16_t> (getLong(columnIndex));
}

int16_t MysqlResultSet::getShort(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getShort(getColumnIndex(columnName));
}

int16_t MysqlResultSet::getShortDefault(uint32_t columnIndex,
		int16_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getShort(columnIndex);
}

int16_t MysqlResultSet::getShortDefault(const std::string& columnName,
		int16_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getShortDefault(getColumnIndex(columnName), defaultValue);
}

uint16_t MysqlResultSet::getUShort(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return static_cast<uint16_t> (getULong(columnIndex));
}

uint16_t MysqlResultSet::getUShort(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getUShort(getColumnIndex(columnName));
}

uint16_t MysqlResultSet::getUShortDefault(uint32_t columnIndex,
		uint16_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getUShort(columnIndex);
}

uint16_t MysqlResultSet::getUShortDefault(const std::string& columnName,
		uint16_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getUShortDefault(getColumnIndex(columnName), defaultValue);
}

int32_t MysqlResultSet::getInt(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return static_cast<int32_t> (getLong(columnIndex));
}

int32_t MysqlResultSet::getInt(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getInt(getColumnIndex(columnName));
}

int32_t MysqlResultSet::getIntDefault(uint32_t columnIndex,
		int32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getInt(columnIndex);
}

int32_t MysqlResultSet::getIntDefault(const std::string& columnName,
		int32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getIntDefault(getColumnIndex(columnName), defaultValue);
}

uint32_t MysqlResultSet::getUInt(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return static_cast<uint32_t> (getULong(columnIndex));
}

uint32_t MysqlResultSet::getUInt(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getUInt(getColumnIndex(columnName));
}

uint32_t MysqlResultSet::getUIntDefault(uint32_t columnIndex,
		uint32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getUInt(columnIndex);
}

uint32_t MysqlResultSet::getUIntDefault(const std::string& columnName,
		uint32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getUIntDefault(getColumnIndex(columnName), defaultValue);
}

int64_t MysqlResultSet::getLong(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	columnIndex--;

	if (NULL == row_[columnIndex])
	{
		THROW2(SqlException, std::string("Null field, index: ") + mxcore::Integer(columnIndex).toString());
	}

	return mxcore::Long::fromString(row_[columnIndex]).getValue();
}

int64_t MysqlResultSet::getLong(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getLong(getColumnIndex(columnName));
}

int64_t MysqlResultSet::getLongDefault(uint32_t columnIndex,
		int64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getLong(columnIndex);
}

int64_t MysqlResultSet::getLongDefault(const std::string& columnName,
		int64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getLongDefault(getColumnIndex(columnName), defaultValue);
}

uint64_t MysqlResultSet::getULong(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	columnIndex--;

	if (NULL == row_[columnIndex])
	{
		THROW2(SqlException, std::string("Null field, index: ") + mxcore::Integer(columnIndex).toString());
	}

	return mxcore::ULong::fromString(row_[columnIndex]).getValue();
}

uint64_t MysqlResultSet::getULong(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getULong(getColumnIndex(columnName));
}

uint64_t MysqlResultSet::getULongDefault(uint32_t columnIndex,
		uint64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getULong(columnIndex);
}

uint64_t MysqlResultSet::getULongDefault(const std::string& columnName,
		uint64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getULongDefault(getColumnIndex(columnName), defaultValue);
}

float MysqlResultSet::getFloat(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	columnIndex--;

	if (NULL == row_[columnIndex])
	{
		THROW2(SqlException, std::string("Null field, index: ") + mxcore::Integer(columnIndex).toString());
	}

	return mxcore::Float::fromString(row_[columnIndex]).getValue();
}

float MysqlResultSet::getFloat(const std::string& columnName)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getFloat(getColumnIndex(columnName));
}

float MysqlResultSet::getFloatDefault(uint32_t columnIndex, float defaultValue)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return isNull(columnIndex) ? defaultValue : getFloat(columnIndex);
}

float MysqlResultSet::getFloatDefault(const std::string& columnName,
		float defaultValue)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getFloatDefault(columnName, defaultValue);
}

double MysqlResultSet::getDouble(uint32_t columnIndex) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	columnIndex--;

	if (NULL == row_[columnIndex])
	{
		THROW2(SqlException, std::string("Null field, index: ") + mxcore::Integer(columnIndex).toString());
	}

	return mxcore::Double::fromString(row_[columnIndex]).getValue();
}

double MysqlResultSet::getDouble(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getDouble(getColumnIndex(columnName));
}

double MysqlResultSet::getDoubleDefault(uint32_t columnIndex,
		double defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getDouble(columnIndex);
}

double MysqlResultSet::getDoubleDefault(const std::string& columnName,
		double defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getDoubleDefault(getColumnIndex(columnName), defaultValue);
}

std::string MysqlResultSet::getString(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	columnIndex--;

	if (NULL == row_[columnIndex] || fieldLengths_[columnIndex] <= 0)
	{
		return "";
	}

	return std::string(row_[columnIndex], fieldLengths_[columnIndex]);
}

std::string MysqlResultSet::getString(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getString(getColumnIndex(columnName));
}

std::string MysqlResultSet::getBytes(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	columnIndex--;

	if (NULL == row_[columnIndex] || fieldLengths_[columnIndex] <= 0)
	{
		return "";
	}

	return std::string(row_[columnIndex], fieldLengths_[columnIndex]);
}

std::string MysqlResultSet::getBytes(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getBytes(getColumnIndex(columnName));
}

std::ostream& MysqlResultSet::getBlob(uint32_t columnIndex, std::ostream& out)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	columnIndex--;

	if (NULL == row_[columnIndex] || fieldLengths_[columnIndex] <= 0)
	{
		return out;
	}

	return out.write(row_[columnIndex], fieldLengths_[columnIndex]);
}

std::ostream& MysqlResultSet::getBlob(const std::string& columnName,
		std::ostream& out) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != row_);

	return getBlob(getColumnIndex(columnName), out);
}

uint32_t MysqlResultSet::getColumnIndex(const std::string& columnName) const
		throw (SqlException&)
{
	uint32_t columnIndex = findColumn(columnName);
	if (0 == columnIndex)
	{
		THROW2(SqlException, std::string("Unknown column: [") + columnName + "]");
	}

	return columnIndex;
}

} // namespace mxsql
