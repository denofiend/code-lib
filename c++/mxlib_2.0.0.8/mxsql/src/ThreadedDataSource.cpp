/*
 * ThreadedDataSource.cpp
 *
 *  Created on: 2011-4-26
 *      Author: zhaojiangang
 */

#include <iostream>
#include <mxcore/Assert.h>
#include <mxcore/LoggerFactory.h>
#include <mxsql/ThreadedDataSource.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

static inline mxcore::Logger& logger(void)
{
	return mxcore::LoggerFactory::getInstance().getLogger("mxsql");
}

HeartbeatThread::HeartbeatThread(ThreadedDataSource* dataSource) :
	mxcore::Thread(NULL), canRun_(true), dataSource_(dataSource)
{
}

HeartbeatThread::~HeartbeatThread(void)
{
}

void HeartbeatThread::run(void)
{
	if (logger().isTraceEnable())
	{
		logger().trace(">>> HeartbeatThread::run [%s:%s]\n",
				dataSource_->config_.dbConfig_.host_.c_str(),
				dataSource_->config_.dbConfig_.dbname_.c_str());
	}

	while (canRun_)
	{
		/* 1 获取所有空闲连接 */
		std::list<DataSourceSqlConnection*> freeConnections;
		{
			mxcore::Lock::ScopeLock sl(dataSource_->connectionLock_);
			freeConnections = dataSource_->freeConnectionPool_;
			dataSource_->freeConnectionPool_.clear();
		}

		if (logger().isDebugEnable())
		{
			logger().debug(
					"DataSource [%s:%s] freeConnections %u, idleCount %u\n",
					dataSource_->config_.dbConfig_.host_.c_str(),
					dataSource_->config_.dbConfig_.dbname_.c_str(),
					freeConnections.size(), dataSource_->config_.idleCount_);
		}

		while (!freeConnections.empty())
		{
			DataSourceSqlConnection* connection = freeConnections.front();
			freeConnections.pop_front();

			bool isValid = doHeartbeat(connection);

			if (isValid)
			{
				isValid = doIdleCheck(connection);
			}

			if (!isValid)
			{
				/* 检查失败，从连接池里把该连接删除 */
				{
					mxcore::Lock::ScopeLock sl(dataSource_->connectionLock_);
					dataSource_->connectionPool_.erase(connection);
				}

				if (logger().isDebugEnable())
				{
					logger().debug("DataSource [%s:%s] close connection(%p)\n",
							dataSource_->config_.dbConfig_.host_.c_str(),
							dataSource_->config_.dbConfig_.dbname_.c_str(),
							connection);
				}
				delete connection;
			}
			else
			{
				/* 归还到空闲连结池,并通知等待线程 */
				mxcore::Lock::ScopeLock sl(dataSource_->connectionLock_);
				dataSource_->freeConnectionPool_.push_back(connection);
				dataSource_->connectionCond_.notify();
			}
		}

		/* 休眠 */
		if (logger().isTraceEnable())
		{
			logger().trace(">>> DataSource [%s:%s] wait heartbeat %u ms\n",
					dataSource_->config_.dbConfig_.host_.c_str(),
					dataSource_->config_.dbConfig_.dbname_.c_str(),
					dataSource_->config_.heartbeatInterval_);
		}

		mxcore::Lock::ScopeLock sl(lock_);
		cond_.wait(lock_, dataSource_->config_.heartbeatInterval_);

		if (logger().isTraceEnable())
		{
			logger().trace("<<< DataSource [%s:%s] wait heartbeat\n",
					dataSource_->config_.dbConfig_.host_.c_str(),
					dataSource_->config_.dbConfig_.dbname_.c_str());
		}
	}

	if (logger().isTraceEnable())
	{
		logger().trace("<<< HeartbeatThread::run [%s:%s]\n",
				dataSource_->config_.dbConfig_.host_.c_str(),
				dataSource_->config_.dbConfig_.dbname_.c_str());
	}
}

void HeartbeatThread::notifyStop(void)
{
	mxcore::Lock::ScopeLock sl(lock_);
	canRun_ = false;
	cond_.notify();
}

bool HeartbeatThread::doHeartbeat(DataSourceSqlConnection* connection)
{
	if (logger().isDebugEnable())
	{
		logger().debug("DataSource [%s:%s] heartbeat sql [%s]\n",
				dataSource_->config_.dbConfig_.host_.c_str(),
				dataSource_->config_.dbConfig_.dbname_.c_str(),
				dataSource_->config_.heartbeatSql_.c_str());
	}

	if (!dataSource_->config_.heartbeatSql_.empty())
	{
		try
		{
			std::auto_ptr<SqlStatement> stmt(connection->createStatement());

			if (stmt->execute(dataSource_->config_.heartbeatSql_))
			{
				std::auto_ptr<SqlResultSet> rs(stmt->getResultSet());
			}

			if (logger().isDebugEnable())
			{
				logger().debug("DataSource [%s:%s] heartbeat [%s] successed\n",
						dataSource_->config_.dbConfig_.host_.c_str(),
						dataSource_->config_.dbConfig_.dbname_.c_str(),
						dataSource_->config_.heartbeatSql_.c_str());
			}
		} catch (SqlException& e)
		{
			if (logger().isDebugEnable())
			{
				logger().debug(
						"DataSource [%s:%s] heartbeat [%s] failed [%d:%s]\n",
						dataSource_->config_.dbConfig_.host_.c_str(),
						dataSource_->config_.dbConfig_.dbname_.c_str(),
						dataSource_->config_.heartbeatSql_.c_str(),
						e.getErrorCode(), e.getMessage().c_str());
			}

			return false;
		}
	}
	return true;
}

bool HeartbeatThread::doIdleCheck(DataSourceSqlConnection* connection)
{
	time_t nowT = time(0);

	/* 超时并且连接数量大于idleCount,此连接可以关 */
	return !((nowT - connection->lastActiveTime_
			> dataSource_->config_.idleTimeout_)
			&& (dataSource_->connectionPool_.size()
					> dataSource_->config_.idleCount_));
}

SqlConnectionProxy::SqlConnectionProxy(DataSourceSqlConnection* connection,
		ThreadedDataSource* dataSource) :
	dataSource_(dataSource), connectionImpl_(connection)
{
}

SqlConnectionProxy::~SqlConnectionProxy(void)
{
	dataSource_->returnConnection(connectionImpl_);
}

void SqlConnectionProxy::close(void)
{
	if (!isClosed_)
	{
		isClosed_ = true;
	}
}

bool SqlConnectionProxy::isClosed(void) const
{
	return isClosed_;
}

void SqlConnectionProxy::setCharset(const std::string& charset)
		throw (SqlException&)
{
	connectionImpl_->setCharset(charset);
}

std::string SqlConnectionProxy::getCharset(void) throw (SqlException&)
{
	return connectionImpl_->getCharset();
}

bool SqlConnectionProxy::getAutoCommit(void) const
{
	return connectionImpl_->getAutoCommit();
}

void SqlConnectionProxy::setAutoCommit(bool autoCommit) throw (SqlException&)
{
	return connectionImpl_->setAutoCommit(autoCommit);
}

enum_transaction_isolation SqlConnectionProxy::getTransactionIsolation(void) const
{
	return connectionImpl_->getTransactionIsolation();
}

void SqlConnectionProxy::setTransactionIsolation(
		enum_transaction_isolation level) throw (SqlException&)
{
	connectionImpl_->setTransactionIsolation(level);
}

SqlStatement* SqlConnectionProxy::createStatement(void) throw (SqlException&)
{
	return connectionImpl_->createStatement();
}

SqlPreparedStatement* SqlConnectionProxy::preparedStatement(
		const std::string& sql) throw (SqlException&)
{
	return connectionImpl_->preparedStatement(sql);
}

SqlTransaction* SqlConnectionProxy::beginTransaction(void)
		throw (SqlException&)
{
	return connectionImpl_->beginTransaction();
}

uint64_t SqlConnectionProxy::getInsertId(void) const
{
	return connectionImpl_->getInsertId();
}

DataSourceSqlConnection::DataSourceSqlConnection(SqlConnection* connection) :
	refCount_(0), lastActiveTime_(time(0)), threadId_(0),
			connection_(connection)

{
}

DataSourceSqlConnection::~DataSourceSqlConnection(void)
{
	delete connection_;
}

void DataSourceSqlConnection::close(void)
{
	connection_->close();
}

bool DataSourceSqlConnection::isClosed(void) const
{
	return connection_->isClosed();
}

void DataSourceSqlConnection::setCharset(const std::string& charset)
		throw (SqlException&)
{
	connection_->setCharset(charset);
}

std::string DataSourceSqlConnection::getCharset(void) throw (SqlException&)
{
	return connection_->getCharset();
}

bool DataSourceSqlConnection::getAutoCommit(void) const
{
	return connection_->getAutoCommit();
}

void DataSourceSqlConnection::setAutoCommit(bool autoCommit)
		throw (SqlException&)
{
	connection_->setAutoCommit(autoCommit);
}

enum_transaction_isolation DataSourceSqlConnection::getTransactionIsolation(
		void) const
{
	return connection_->getTransactionIsolation();
}

void DataSourceSqlConnection::setTransactionIsolation(
		enum_transaction_isolation level) throw (SqlException&)
{
	connection_->setTransactionIsolation(level);
}

SqlStatement* DataSourceSqlConnection::createStatement(void)
		throw (SqlException&)
{
	return connection_->createStatement();
}

SqlPreparedStatement* DataSourceSqlConnection::preparedStatement(
		const std::string& sql) throw (SqlException&)
{
	return connection_->preparedStatement(sql);
}

SqlTransaction* DataSourceSqlConnection::beginTransaction(void)
		throw (SqlException&)
{
	return connection_->beginTransaction();
}

uint64_t DataSourceSqlConnection::getInsertId(void) const
{
	return connection_->getInsertId();
}

ThreadedDataSource::ThreadedDataSource(const DataSource::Config& config,
		SqlConnectionFactory* connectionFactory) :
	config_(config), connectionFactory_(connectionFactory),
			heartbeatThread_(NULL)
{
	MX_ASSERT(NULL != connectionFactory_);
}

ThreadedDataSource::~ThreadedDataSource()
{
	if (NULL != heartbeatThread_)
	{
		heartbeatThread_->notifyStop();
		heartbeatThread_->join();
		delete heartbeatThread_;
	}

	freeConnectionPool_.clear();
	usedConnections_.clear();

	for (std::set<DataSourceSqlConnection*>::const_iterator it =
			connectionPool_.begin(); it != connectionPool_.end(); it++)
	{
		delete *it;
	}
	connectionPool_.clear();
}

void ThreadedDataSource::init(void)
{
	if (NULL == heartbeatThread_)
	{
		mxcore::Lock::ScopeLock sl(initLock_);

		if (NULL == heartbeatThread_)
		{
			heartbeatThread_ = new HeartbeatThread(this);
			heartbeatThread_->start();
		}
	}
}

std::auto_ptr<SqlConnection> ThreadedDataSource::getConnection(void)
		throw (SqlException&)
{
	long threadId = mxcore::Thread::current();
	init();

	mxcore::Lock::ScopeLock sl(connectionLock_);

	DataSourceSqlConnection* connection = borrowConnection(threadId);

	if (NULL == connection)
	{
		THROW2(SqlException, "No idle connection");
	}

	return std::auto_ptr<SqlConnection>(
			new SqlConnectionProxy(connection, this));
}

void ThreadedDataSource::returnConnection(DataSourceSqlConnection* connection)
{
	time_t lastActiveTime = time(NULL);
	mxcore::Lock::ScopeLock sl(connectionLock_);

	connection->refCount_--;

	if (0 == connection->refCount_)
	{
		/* 从应用连结池中删除该连接 */
		usedConnections_.erase(connection->threadId_);

		/* 修改最后活跃时间 */
		connection->lastActiveTime_ = lastActiveTime;

		/* 添加到空闲连结池 */
		freeConnectionPool_.push_back(connection);

		/* 通知等待线程 */
		connectionCond_.notify();
	}
}

DataSourceSqlConnection* ThreadedDataSource::borrowConnection(long threadId)
{
	/* 已经有连结被该线程应用 */

	{
		std::map<long, DataSourceSqlConnection*>::iterator it =
				usedConnections_.find(threadId);

		if (it != usedConnections_.end())
		{
			DataSourceSqlConnection* connection = it->second;
			connection->refCount_++;
			return connection;
		}
	}

	/* 已经没有空闲连结，但是还没有到最大连结数 */
	if (freeConnectionPool_.empty() && connectionPool_.size()
			< config_.maxCount_)
	{
		/* 新建连结 */
		try
		{
			SqlConnection* newConnection = connectionFactory_->newConnection(
					config_.dbConfig_);

			/* 添加连接到连结池 */
			DataSourceSqlConnection* connection = new DataSourceSqlConnection(
					newConnection);
			connectionPool_.insert(connection);
			freeConnectionPool_.push_back(connection);

			if (logger().isDebugEnable())
			{
				logger().debug("DataSource [%s:%s] new connection(%p)\n",
						config_.dbConfig_.host_.c_str(),
						config_.dbConfig_.dbname_.c_str(), connection);
			}
		} catch (SqlException& e)
		{
			/* TODO log */
		}
	}

	/* 没有空闲连接，等待 */
	if (freeConnectionPool_.empty() && config_.waitTimeout_ > 0)
	{
		connectionCond_.wait(connectionLock_, config_.waitTimeout_);
	}

	/* 有空闲的连结 */
	if (!freeConnectionPool_.empty())
	{
		/* 从空闲线程列表里移除该连结 */
		DataSourceSqlConnection* connection = freeConnectionPool_.front();
		freeConnectionPool_.pop_front();

		/* 插入到应用连结列表，并增加引用计数器 */
		connection->refCount_++;
		connection->threadId_ = threadId;
		usedConnections_.insert(std::make_pair(threadId, connection));

		/* 返回连结 */
		return connection;
	}

	return NULL;
}

} // namespace mxsql
