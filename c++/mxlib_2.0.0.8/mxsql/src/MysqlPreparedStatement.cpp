/*
 *        Name: MysqlPreparedStatement.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Real.h>
#include <mxcore/Assert.h>
#include <mxcore/Number.h>
#include <mxcore/SystemOps.h>
#include <mxcore/NotImplException.h>
#include <mxsql/mysql/MysqlPreparedResultSet.h>
#include <mxsql/mysql/MysqlPreparedStatement.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

MysqlPreparedStatement::MysqlPreparedStatement(MysqlConnection* connection,
		MYSQL_STMT* statement, SqlResultSet::enum_type resultType) :
	connection_(connection), statement_(statement), resultType_(resultType)
{
	numParams_ = ::mysql_stmt_param_count(statement);
	paramsBind_ = new MYSQL_BIND[numParams_];
	::memset(paramsBind_, 0, sizeof(MYSQL_BIND) * numParams_);
}

MysqlPreparedStatement::~MysqlPreparedStatement()
{
	close();
}

bool MysqlPreparedStatement::execute(const std::string& sql)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	THROW2(mxcore::NotImplException, "MysqlPreparedStatement::execute");
}

uint64_t MysqlPreparedStatement::executeUpdate(const std::string& sql)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	THROW2(mxcore::NotImplException, "MysqlPreparedStatement::executeUpdate");
}

SqlResultSet* MysqlPreparedStatement::executeQuery(const std::string& sql)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	THROW2(mxcore::NotImplException, "MysqlPreparedStatement::executeQuery");
}

void MysqlPreparedStatement::close(void)
{
	if (NULL != statement_)
	{
		nullParams_.clear();
		stringParams_.clear();
		istreamsParams_.clear();

		if (NULL != statement_)
		{
			::mysql_stmt_free_result(statement_);
			::mysql_stmt_close(statement_);
			statement_ = NULL;
		}

		if (NULL != paramsBind_)
		{
			delete[] paramsBind_;
			paramsBind_ = NULL;
		}
	}
}

bool MysqlPreparedStatement::isClosed(void) const
{
	return NULL == statement_;
}

SqlResultSet* MysqlPreparedStatement::getResultSet(void) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	return new MysqlPreparedResultSet(connection_, statement_, resultType_);
}

SqlConnection* MysqlPreparedStatement::getConnection(void) const
{
	return (SqlConnection*) connection_;
}

SqlResultSet::enum_type MysqlPreparedStatement::getResultType(void) const
{
	return resultType_;
}

void MysqlPreparedStatement::setResultType(SqlResultSet::enum_type resultType)
{
	resultType_ = resultType;
}

bool MysqlPreparedStatement::execute(void) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	bindParams();

	return executeImpl();
}

uint32_t MysqlPreparedStatement::executeUpdate(void) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	bindParams();

	executeImpl();

	return ::mysql_stmt_affected_rows(statement_);
}

SqlResultSet* MysqlPreparedStatement::executeQuery(void) throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	bindParams();

	if (!executeImpl())
	{
		THROW2(SqlException, "Invalid execute query: no result");
	}

	return getResultSet();
}

void MysqlPreparedStatement::setNull(uint32_t index) throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);

	if (isParamSet(index))
	{
		THROW2(SqlException, std::string("Param already set: ") + mxcore::Integer(index).toString());
	}

	paramsBind_[index - 1].buffer_type = MYSQL_TYPE_NULL;
	paramsBind_[index - 1].buffer = NULL;
	paramsBind_[index - 1].is_null = 0;
	paramsBind_[index - 1].length = 0;

	nullParams_[index] = index;
}

void MysqlPreparedStatement::setShort(uint32_t index, int16_t value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(uint16_t), MYSQL_TYPE_SHORT, false);
}

void MysqlPreparedStatement::setUShort(uint32_t index, uint16_t value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(uint16_t), MYSQL_TYPE_SHORT, true);
}

void MysqlPreparedStatement::setInt(uint32_t index, int32_t value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(int32_t), MYSQL_TYPE_LONG, false);
}

void MysqlPreparedStatement::setUInt(uint32_t index, uint32_t value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(uint32_t), MYSQL_TYPE_LONG, true);
}

void MysqlPreparedStatement::setLong(uint32_t index, int64_t value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(int64_t), MYSQL_TYPE_LONGLONG, false);
}

void MysqlPreparedStatement::setULong(uint32_t index, uint64_t value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(uint64_t), MYSQL_TYPE_LONGLONG, true);
}

void MysqlPreparedStatement::setFloat(uint32_t index, float value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(float), MYSQL_TYPE_FLOAT, false);
}

void MysqlPreparedStatement::setDouble(uint32_t index, double value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setParam(index, &value, sizeof(double), MYSQL_TYPE_DOUBLE, false);
}

void MysqlPreparedStatement::setString(uint32_t index, const char* value,
		size_t len) throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);

	if (NULL == value)
	{
		setNull(index);
	}
	else
	{
		setParam(index, value, len, MYSQL_TYPE_STRING, false);
	}
}

void MysqlPreparedStatement::setString(uint32_t index, const std::string& value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setString(index, value.c_str(), value.size());
}

void MysqlPreparedStatement::setBytes(uint32_t index, const void* value,
		size_t len) throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);

	if (NULL == value)
	{
		setNull(index);
	}
	else
	{
		setParam(index, value, len, MYSQL_TYPE_BLOB, false);
	}
}

void MysqlPreparedStatement::setBytes(uint32_t index, const std::string& value)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);
	setBytes(index, value.c_str(), value.size());
}

std::istream& MysqlPreparedStatement::setBlob(uint32_t index, std::istream& in)
		throw (SqlException&)
{
	MX_ASSERT(index > 0 && index <= numParams_);

	paramsBind_[index - 1].buffer_type = MYSQL_TYPE_LONG_BLOB;
	paramsBind_[index - 1].buffer = NULL;
	paramsBind_[index - 1].buffer_length = 0;
	paramsBind_[index - 1].is_null = 0;
	paramsBind_[index - 1].length = 0;
	istreamsParams_[index] = &in;
	return in;
}

bool MysqlPreparedStatement::isParamSet(uint32_t index) const
{
	MX_ASSERT(index > 0 && index <= numParams_);

	return nullParams_.find(index) != nullParams_.end() || stringParams_.find(
			index) != stringParams_.end() || istreamsParams_.find(index)
			!= istreamsParams_.end();
}

void MysqlPreparedStatement::setParam(uint32_t index, const void* data,
		size_t len, enum enum_field_types type, bool isUnsigned)
{
	if (isParamSet(index))
	{
		THROW2(SqlException, std::string("Param already set: ") + mxcore::Integer(index).toString());
	}

	stringParams_[index].append((const char*) data, len);

	paramsBind_[index - 1].buffer_type = type;
	paramsBind_[index - 1].buffer = (void*) stringParams_[index].c_str();
	paramsBind_[index - 1].buffer_length = stringParams_[index].size();
	paramsBind_[index - 1].is_null = 0;
	paramsBind_[index - 1].length = 0;
	paramsBind_[index - 1].is_unsigned = isUnsigned ? 1 : 0;
}

static const uint32_t READ_BUF_LEN = 4096;

void MysqlPreparedStatement::bindParams(void) throw (SqlException&)
{
	for (uint32_t i = 1; i <= numParams_; i++)
	{
		if (!isParamSet(i))
		{
			THROW2(SqlException, std::string("Param not set: ") + mxcore::Integer(i).toString());
		}
	}

	if (::mysql_stmt_bind_param(statement_, paramsBind_))
	{
		THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
	}

	char tmp[READ_BUF_LEN + 1];

	for (std::map<uint32_t, std::istream*>::iterator it =
			istreamsParams_.begin(); it != istreamsParams_.end(); it++)
	{
		std::istream* in = it->second;

		in->read(tmp, READ_BUF_LEN);

		uint32_t index = it->first - 1;

		while (!in->eof() && in->good())
		{
			if (::mysql_stmt_send_long_data(statement_, index, tmp,
					in->gcount()))
			{
				THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
			}

			in->read(tmp, READ_BUF_LEN);
		}

		if (in->eof())
		{
			if (::mysql_stmt_send_long_data(statement_, index, tmp,
					in->gcount()))
			{
				THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
			}
		}
		else
		{
			THROW3(SqlException, "Read from input error", mxos::getLastError());
		}
	}
}

bool MysqlPreparedStatement::executeImpl(void) throw (SqlException&)
{
	if (0 != ::mysql_stmt_execute(statement_))
	{
		THROW3(SqlException, ::mysql_stmt_error(statement_), ::mysql_stmt_errno(statement_));
	}

	MYSQL_RES* resultMetadata = ::mysql_stmt_result_metadata(statement_);

	if (NULL != resultMetadata)
	{
		::mysql_free_result(resultMetadata);
		return true;
	}

	return false;
}

} // namespace mxsql
