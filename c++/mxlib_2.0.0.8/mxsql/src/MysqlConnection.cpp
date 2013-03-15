/*
 *        Name: MysqlConnection.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Assert.h>
#include <mxsql/mysql/MysqlStatement.h>
#include <mxsql/mysql/MysqlConnection.h>
#include <mxsql/mysql/MysqlTransaction.h>
#include <mxsql/mysql/MysqlPreparedStatement.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

MysqlConnection::MysqlConnection(void) :
	connection_(NULL), autoCommit_(true), txLevel_(TRANSACTION_REPEATABLE_READ)
{
}

MysqlConnection::~MysqlConnection()
{
	close();
}

void MysqlConnection::open(const DbConfig& dbConfig) throw (SqlException&)
//const std::string& host, unsigned int port,
//		const std::string& username, const std::string& password,
//		const std::string& db, bool autoCommit, unsigned int connTimeout)
//		throw (SqlException&)
{
	close();
	connection_ = ::mysql_init(NULL);

	::mysql_options(connection_, MYSQL_OPT_CONNECT_TIMEOUT,
			(const char*) &dbConfig.connectTimeout_);

	if (NULL == connection_)
	{
		std::string message("Can not connect to [");
		message += dbConfig.host_;
		message += "]";
		THROW2(SqlException, message);
	}

	// 设置了CLIENT_MULTI_STATEMENTS是为了调存储过程取出多行结果
	if (NULL == ::mysql_real_connect(connection_, dbConfig.host_.c_str(),
			dbConfig.username_.c_str(), dbConfig.password_.c_str(),
			dbConfig.dbname_.c_str(), dbConfig.port_, NULL, CLIENT_MULTI_STATEMENTS))
	{

		std::string message(::mysql_error(connection_));
		int errorCode = ::mysql_errno(connection_);
		::mysql_close(connection_);
		connection_ = NULL;
		THROW3(SqlException, message, errorCode);
	}

	try
	{
		setAutoCommit(dbConfig.autoCommit_);
		setCharset(dbConfig.charset_);
		setTransactionIsolation(TRANSACTION_REPEATABLE_READ);
	} catch (SqlException& e)
	{
		close();
		throw e;
	}
}

void MysqlConnection::close(void)
{
	if (NULL != connection_)
	{
		::mysql_close(connection_);
		connection_ = NULL;
	}
}

bool MysqlConnection::isClosed(void) const
{
	return NULL == connection_;
}

void MysqlConnection::setCharset(const std::string& charset)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	::mysql_set_character_set(connection_, charset.c_str());
}

std::string MysqlConnection::getCharset(void) throw (SqlException&)
{
	MY_CHARSET_INFO info;
	MX_ASSERT(!isClosed());
	::mysql_get_character_set_info(connection_, &info);

	if (info.csname != NULL)
	{
		return std::string(info.csname);
	}
	return "";
}

bool MysqlConnection::getAutoCommit(void) const
{
	return autoCommit_;
}

void MysqlConnection::setAutoCommit(bool autoCommit) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	autoCommit_ = autoCommit;

	if (0 != ::mysql_autocommit(connection_, autoCommit_))
	{
		THROW3(SqlException, ::mysql_error(connection_), ::mysql_errno(connection_));
	}
}

enum_transaction_isolation MysqlConnection::getTransactionIsolation(void) const
{
	return txLevel_;
}

void MysqlConnection::setTransactionIsolation(enum_transaction_isolation level)
		throw (SqlException&)
{
	std::string q;
	MX_ASSERT(!isClosed());

	switch (level)
	{
	case TRANSACTION_SERIALIZABLE:
		q = "SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE";
		break;
	case TRANSACTION_REPEATABLE_READ:
		q = "SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ";
		break;
	case TRANSACTION_READ_COMMITTED:
		q = "SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED";
		break;
	case TRANSACTION_READ_UNCOMMITTED:
		q = "SET SESSION TRANSACTION ISOLATION LEVEL READ UNCOMMITTED";
		break;
	default:
		THROW2(SqlException, "Invalid transaction level");
	}

	txLevel_ = level;

	if (0 != ::mysql_real_query(connection_, q.c_str(), q.size()))
	{
		THROW3(SqlException, ::mysql_error(connection_), ::mysql_errno(connection_));
	}
}

SqlStatement* MysqlConnection::createStatement(void) throw (SqlException&)
{
	MX_ASSERT(!isClosed());
	return new MysqlStatement(this, SqlResultSet::TYPE_SCROLL_INSENSITIVE);
}

SqlPreparedStatement* MysqlConnection::preparedStatement(const std::string& sql)
		throw (SqlException&)
{
	MX_ASSERT(!isClosed());

	MYSQL_STMT* stmt = ::mysql_stmt_init(connection_);

	if (NULL == stmt)
	{
		THROW3(SqlException, ::mysql_error(connection_), ::mysql_errno(connection_));
	}

	if (0 != ::mysql_stmt_prepare(stmt, sql.c_str(), sql.size()))
	{
		std::string message(::mysql_stmt_error(stmt));
		int errorCode = ::mysql_stmt_errno(stmt);

		::mysql_stmt_close(stmt);
		THROW3(SqlException, message, errorCode);
	}

	return new MysqlPreparedStatement(this, stmt,
			SqlResultSet::TYPE_SCROLL_INSENSITIVE);
}

void MysqlConnection::commit(void) throw (SqlException&)
{
	const std::string q("COMMIT");

	MX_ASSERT(!isClosed());

	if (0 != ::mysql_real_query(connection_, q.c_str(), q.size()))
	{
		THROW3(SqlException, ::mysql_error(connection_), ::mysql_errno(connection_));
	}
}

void MysqlConnection::rollback(void) throw (SqlException&)
{
	const std::string q("ROLLBACK");

	MX_ASSERT(!isClosed());

	if (0 != ::mysql_real_query(connection_, q.c_str(), q.size()))
	{
		THROW3(SqlException, ::mysql_error(connection_), ::mysql_errno(connection_));
	}
}

SqlTransaction* MysqlConnection::beginTransaction(void) throw (SqlException&)
{
	const std::string q("BEGIN");

	MX_ASSERT(!isClosed());

	if (0 != ::mysql_real_query(connection_, q.c_str(), q.size()))
	{
		THROW3(SqlException, ::mysql_error(connection_), ::mysql_errno(connection_));
	}

	return new MysqlTransaction(this);
}

uint64_t MysqlConnection::getInsertId(void) const
{
	MX_ASSERT(!isClosed());

	return ::mysql_insert_id(connection_);
}

MYSQL* MysqlConnection::getConnection(void) const
{
	return connection_;
}

} // namespace mxsql
