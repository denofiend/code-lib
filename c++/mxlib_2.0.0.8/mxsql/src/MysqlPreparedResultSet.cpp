/*
 *        Name: MysqlPreparedResultSet.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <iostream>
#include <mxcore/Assert.h>
#include <mxsql/mysql/MysqlPreparedResultSet.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

MysqlPreparedResultSet::MysqlPreparedResultSet(MysqlConnection* connection,
		MYSQL_STMT* stmt, SqlResultSet::enum_type resultType) :
	connection_(connection), statement_(stmt), resultType_(resultType)
{
	numFields_ = ::mysql_stmt_field_count(statement_);
	resultBind_ = new MYSQL_BIND[numFields_];

	::memset(resultBind_, 0, sizeof(MYSQL_BIND) * numFields_);

	::mysql_stmt_bind_result(statement_, resultBind_);
	::mysql_stmt_store_result(statement_);

	MYSQL_RES* resultMeta = ::mysql_stmt_result_metadata(statement_);

	numRows_ = ::mysql_stmt_num_rows(statement_);
	rowPos_ = 0;

	for (uint32_t i = 0; i < numFields_; i++)
	{
		columnMap_[::mysql_fetch_field(resultMeta)->name] = i + 1;
	}

	::mysql_free_result(resultMeta);
}

MysqlPreparedResultSet::~MysqlPreparedResultSet(void)
{
	close();
}

void MysqlPreparedResultSet::close(void)
{
	if (NULL != statement_)
	{
		if (NULL != resultBind_)
		{
			delete[] resultBind_;
			resultBind_ = NULL;
		}
		statement_ = NULL;
	}
}

bool MysqlPreparedResultSet::isClosed(void) const
{
	return NULL == statement_;
}

uint32_t MysqlPreparedResultSet::getNumFields(void) const
{
	return numFields_;
}

bool MysqlPreparedResultSet::next(void) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	if (rowPos_ >= numRows_)
	{
		return false;
	}

	rowPos_++;
	int err = ::mysql_stmt_fetch(statement_);

	if (0 != err && MYSQL_DATA_TRUNCATED != err)
	{
		THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
	}

	return true;
}

bool MysqlPreparedResultSet::isNull(uint32_t columnIndex) const
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return resultBind_[columnIndex - 1].is_null;
}

bool MysqlPreparedResultSet::isNull(const std::string& columnName) const
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return isNull(getColumnIndex(columnName));
}

uint32_t MysqlPreparedResultSet::findColumn(const std::string& columnName) const
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

int16_t MysqlPreparedResultSet::getShort(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	int16_t ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_SHORT, false);

	return ret;
}

int16_t MysqlPreparedResultSet::getShort(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getShort(getColumnIndex(columnName));
}

int16_t MysqlPreparedResultSet::getShortDefault(uint32_t columnIndex,
		int16_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getShort(columnIndex);
}

int16_t MysqlPreparedResultSet::getShortDefault(const std::string& columnName,
		int16_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getShortDefault(getColumnIndex(columnName), defaultValue);
}

uint16_t MysqlPreparedResultSet::getUShort(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	uint16_t ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_SHORT, true);

	return ret;
}

uint16_t MysqlPreparedResultSet::getUShort(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getUShort(getColumnIndex(columnName));
}

uint16_t MysqlPreparedResultSet::getUShortDefault(uint32_t columnIndex,
		uint16_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getUShort(columnIndex);
}

uint16_t MysqlPreparedResultSet::getUShortDefault(
		const std::string& columnName, uint16_t defaultValue)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getUShortDefault(getColumnIndex(columnName), defaultValue);
}

int32_t MysqlPreparedResultSet::getInt(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	int32_t ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_LONG, false);

	return ret;
}

int32_t MysqlPreparedResultSet::getInt(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getInt(getColumnIndex(columnName));
}

int32_t MysqlPreparedResultSet::getIntDefault(uint32_t columnIndex,
		int32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getInt(columnIndex);
}

int32_t MysqlPreparedResultSet::getIntDefault(const std::string& columnName,
		int32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getIntDefault(getColumnIndex(columnName), defaultValue);
}

uint32_t MysqlPreparedResultSet::getUInt(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	uint32_t ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_LONG, true);

	return ret;
}

uint32_t MysqlPreparedResultSet::getUInt(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getUInt(getColumnIndex(columnName));
}

uint32_t MysqlPreparedResultSet::getUIntDefault(uint32_t columnIndex,
		uint32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getUInt(columnIndex);
}

uint32_t MysqlPreparedResultSet::getUIntDefault(const std::string& columnName,
		uint32_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getUIntDefault(getColumnIndex(columnName), defaultValue);
}

int64_t MysqlPreparedResultSet::getLong(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	int64_t ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_LONGLONG, false);

	return ret;
}

int64_t MysqlPreparedResultSet::getLong(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getLong(getColumnIndex(columnName));
}

int64_t MysqlPreparedResultSet::getLongDefault(uint32_t columnIndex,
		int64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getLong(columnIndex);
}

int64_t MysqlPreparedResultSet::getLongDefault(const std::string& columnName,
		int64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getLongDefault(getColumnIndex(columnName), defaultValue);
}

uint64_t MysqlPreparedResultSet::getULong(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	int64_t ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_LONGLONG, true);

	return ret;
}

uint64_t MysqlPreparedResultSet::getULong(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getULong(getColumnIndex(columnName));
}

uint64_t MysqlPreparedResultSet::getULongDefault(uint32_t columnIndex,
		uint64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getULong(columnIndex);
}

uint64_t MysqlPreparedResultSet::getULongDefault(const std::string& columnName,
		uint64_t defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getULongDefault(getColumnIndex(columnName), defaultValue);
}

float MysqlPreparedResultSet::getFloat(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	float ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_FLOAT, false);

	return ret;
}

float MysqlPreparedResultSet::getFloat(const std::string& columnName)
{
	MX_ASSERT(!isClosed());

	return getFloat(getColumnIndex(columnName));
}

float MysqlPreparedResultSet::getFloatDefault(uint32_t columnIndex,
		float defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getFloat(columnIndex);
}

float MysqlPreparedResultSet::getFloatDefault(const std::string& columnName,
		float defaultValue)
{
	MX_ASSERT(!isClosed());

	return getFloatDefault(getColumnIndex(columnName), defaultValue);
}

double MysqlPreparedResultSet::getDouble(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	double ret = 0;

	getValue(columnIndex, &ret, sizeof(ret), MYSQL_TYPE_DOUBLE, false);

	return ret;
}

double MysqlPreparedResultSet::getDouble(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getDouble(getColumnIndex(columnName));
}

double MysqlPreparedResultSet::getDoubleDefault(uint32_t columnIndex,
		double defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	return isNull(columnIndex) ? defaultValue : getDouble(columnIndex);
}

double MysqlPreparedResultSet::getDoubleDefault(const std::string& columnName,
		double defaultValue) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getFloatDefault(getColumnIndex(columnName), defaultValue);
}

static const uint32_t READ_BUF_LEN = 4096;

std::string MysqlPreparedResultSet::getString(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	MYSQL_BIND bnd =
	{ 0 };

	std::string ret;

	char buffer[READ_BUF_LEN + 1] =
	{ 0 };

	unsigned long totalLength = 0;
	unsigned long offset = 0;

	bnd.buffer_type = MYSQL_TYPE_STRING;
	bnd.buffer = buffer;
	bnd.buffer_length = READ_BUF_LEN;
	bnd.length = &totalLength;

	do
	{
		int err = ::mysql_stmt_fetch_column(statement_, &bnd, columnIndex - 1,
				offset);

		if (0 != err && MYSQL_NO_DATA != err)
		{
			THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
		}

		uint32_t readLength =
				ret.size() + READ_BUF_LEN > totalLength ? totalLength
						- ret.size() : READ_BUF_LEN;

		offset += readLength;
		ret.append(buffer, readLength);
	} while (offset < totalLength);

	return ret;
}

std::string MysqlPreparedResultSet::getString(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getString(getColumnIndex(columnName));
}

std::string MysqlPreparedResultSet::getBytes(uint32_t columnIndex)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	MYSQL_BIND bnd =
	{ 0 };

	std::string ret;

	char buffer[READ_BUF_LEN + 1] =
	{ 0 };

	unsigned long totalLength = 0;
	unsigned long offset = 0;

	bnd.buffer_type = MYSQL_TYPE_BLOB;
	bnd.buffer = buffer;
	bnd.buffer_length = READ_BUF_LEN;
	bnd.length = &totalLength;

	do
	{
		int err = ::mysql_stmt_fetch_column(statement_, &bnd, columnIndex - 1,
				offset);

		if (0 != err && MYSQL_NO_DATA != err)
		{
			THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
		}

		uint32_t readLength =
				ret.size() + READ_BUF_LEN > totalLength ? totalLength
						- ret.size() : READ_BUF_LEN;

		offset += readLength;
		ret.append(buffer, readLength);
	} while (offset < totalLength);

	return ret;
}

std::string MysqlPreparedResultSet::getBytes(const std::string& columnName)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getBytes(getColumnIndex(columnName));
}

std::ostream& MysqlPreparedResultSet::getBlob(uint32_t columnIndex,
		std::ostream& out) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(columnIndex > 0 && columnIndex <= numFields_);

	MYSQL_BIND bnd =
	{ 0 };

	char buffer[READ_BUF_LEN + 1] =
	{ 0 };
	unsigned long totalLength = 0;
	unsigned long offset = 0;

	bnd.buffer_type = MYSQL_TYPE_LONG_BLOB;
	bnd.buffer = buffer;
	bnd.buffer_length = READ_BUF_LEN;
	bnd.length = &totalLength;

	do
	{
		int err = ::mysql_stmt_fetch_column(statement_, &bnd, columnIndex - 1,
				offset);

		if (0 != err && MYSQL_NO_DATA != err)
		{
			THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
		}

		unsigned long readLength =
				offset + READ_BUF_LEN > totalLength ? totalLength - offset
						: READ_BUF_LEN;

		offset += readLength;
		out.write(buffer, readLength);
	} while (offset < totalLength);

	return out;
}

std::ostream& MysqlPreparedResultSet::getBlob(const std::string& columnName,
		std::ostream& out) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return getBlob(getColumnIndex(columnName), out);
}

uint32_t MysqlPreparedResultSet::getColumnIndex(const std::string& columnName) const
		throw (SqlException&)
{
	uint32_t columnIndex = findColumn(columnName);
	if (0 == columnIndex)
	{
		THROW2(SqlException, std::string("Unknown column: [") + columnName + "]");
	}

	return columnIndex;
}

void MysqlPreparedResultSet::getValue(uint32_t columnIndex, void* value,
		size_t valueSize, enum enum_field_types fieldType, bool isUnsigned)
		throw (SqlException&)
{
	MYSQL_BIND bnd =
	{ 0 };

	bnd.buffer_type = fieldType;
	bnd.buffer = value;
	bnd.buffer_length = valueSize;
	bnd.length = 0;
	bnd.is_unsigned = isUnsigned;

	if (0 != ::mysql_stmt_fetch_column(statement_, &bnd, columnIndex - 1, 0))
	{
		THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
	}
}

} // namespace mxsql
