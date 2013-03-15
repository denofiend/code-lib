/*
 *        Name: MysqlStatement.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxsql/mysql/MysqlResultSet.h>
#include <mxsql/mysql/MysqlStatement.h>
#include <mxsql/mysql/MysqlConnection.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

MysqlStatement::MysqlStatement(MysqlConnection* connection,
		SqlResultSet::enum_type resultType) :
	isClosed_(false), connection_(connection), resultType_(resultType),
			lastUpdateCount_(0)
{
}

MysqlStatement::~MysqlStatement(void)
{
	close();
}

bool MysqlStatement::execute(const std::string& sql) throw (SqlException&)
{
	if (0 != ::mysql_real_query(connection_->getConnection(), sql.c_str(),
			sql.size()))
	{
		THROW3(SqlException, ::mysql_error(connection_->getConnection()), ::mysql_errno(connection_->getConnection()));
	}

	bool ret = ::mysql_field_count(connection_->getConnection()) > 0;
	lastUpdateCount_ = ret ? 0 : ::mysql_affected_rows(
			connection_->getConnection());
	return ret;
}

uint64_t MysqlStatement::executeUpdate(const std::string& sql)
		throw (SqlException&)
{
	if (0 != ::mysql_real_query(connection_->getConnection(), sql.c_str(),
			sql.size()))
	{
		THROW3(SqlException, ::mysql_error(connection_->getConnection()), ::mysql_errno(connection_->getConnection()));
	}

	if (::mysql_field_count(connection_->getConnection()))
	{
		THROW2(SqlException, "Run update but statement return result set");
	}

	lastUpdateCount_ = ::mysql_affected_rows(connection_->getConnection());
	return lastUpdateCount_;
}

SqlResultSet* MysqlStatement::executeQuery(const std::string& sql)
		throw (SqlException&)
{
	lastUpdateCount_ = 0;

	if (0 != ::mysql_real_query(connection_->getConnection(), sql.c_str(),
			sql.size()))
	{
		THROW3(SqlException, ::mysql_error(connection_->getConnection()), ::mysql_errno(connection_->getConnection()));
	}

	return getResultSet();
}

void MysqlStatement::close(void)
{
	isClosed_ = true;
}

bool MysqlStatement::isClosed(void) const
{
	return isClosed_;
}

SqlResultSet* MysqlStatement::getResultSet(void) throw (SqlException&)
{
	MYSQL_RES* result = NULL;

	SqlResultSet::enum_type tmp_type = SqlResultSet::TYPE_FORWARD_ONLY;

	switch (resultType_)
	{
	case SqlResultSet::TYPE_FORWARD_ONLY:
		result = ::mysql_use_result(connection_->getConnection());
		break;
	default:
		tmp_type = SqlResultSet::TYPE_SCROLL_INSENSITIVE;
		result = ::mysql_store_result(connection_->getConnection());
		break;
	}

	if (NULL == result)
	{
		THROW2(SqlException, "No result to return");
	}

	return new MysqlResultSet(result, tmp_type);
}

SqlConnection* MysqlStatement::getConnection(void) const
{
	return connection_;
}

SqlResultSet::enum_type MysqlStatement::getResultType(void) const
{
	return resultType_;
}

void MysqlStatement::setResultType(SqlResultSet::enum_type resultType)
{
	resultType_ = resultType;
}

} // namespace mxsql
