/*
 * ThreadedDataSource.h
 *
 *  Created on: 2011-4-26
 *      Author: zhaojiangang
 */

#ifndef __MXSQL_THREADEDDATASOURCE_H__
#define __MXSQL_THREADEDDATASOURCE_H__

#include <set>
#include <map>
#include <list>
#include <mxsql/DataSource.h>
#include <mxcore/Thread.h>
#include <mxcore/Condition.h>
#include <mxcore/MutexLock.h>
#include <mxsql/SqlConnectionFactory.h>

namespace mxsql
{

class ThreadedDataSource;
class DataSourceSqlConnection;

class HeartbeatThread: public mxcore::Thread
{
public:
	HeartbeatThread(ThreadedDataSource* dataSource);

	virtual ~HeartbeatThread(void);

	virtual void run(void);

	virtual void notifyStop(void);
private:
	bool doHeartbeat(DataSourceSqlConnection* connection);
	bool doIdleCheck(DataSourceSqlConnection* connection);
private:
	bool canRun_;
	mxcore::AutoMutexLock lock_;
	mxcore::Condition cond_;
	ThreadedDataSource* dataSource_;
};

class SqlConnectionProxy: public SqlConnection
{
private:
	friend class ThreadedDataSource;

	SqlConnectionProxy(DataSourceSqlConnection* connection,
			ThreadedDataSource* dataSource);
public:
	virtual ~SqlConnectionProxy(void);

	virtual void close(void);
	virtual bool isClosed(void) const;

	virtual void setCharset(const std::string& charset) throw (SqlException&);
	virtual std::string getCharset(void) throw (SqlException&);

	virtual bool getAutoCommit(void) const;
	virtual void setAutoCommit(bool autoCommit) throw (SqlException&);

	virtual enum_transaction_isolation getTransactionIsolation(void) const;
	virtual void setTransactionIsolation(enum_transaction_isolation level)
			throw (SqlException&);

	virtual SqlStatement* createStatement(void) throw (SqlException&);
	virtual SqlPreparedStatement* preparedStatement(const std::string& sql)
			throw (SqlException&);

	virtual SqlTransaction* beginTransaction(void) throw (SqlException&);
	virtual uint64_t getInsertId(void) const;
private:
	bool isClosed_;
	ThreadedDataSource* dataSource_;
	DataSourceSqlConnection* connectionImpl_;
};

class DataSourceSqlConnection: public SqlConnection
{
	friend class HeartbeatThread;
	friend class ThreadedDataSource;
	DataSourceSqlConnection(SqlConnection* connection);
public:
	virtual ~DataSourceSqlConnection(void);

	virtual void close(void);
	virtual bool isClosed(void) const;

	virtual void setCharset(const std::string& charset) throw (SqlException&);
	virtual std::string getCharset(void) throw (SqlException&);

	virtual bool getAutoCommit(void) const;
	virtual void setAutoCommit(bool autoCommit) throw (SqlException&);

	virtual mxsql::enum_transaction_isolation
	getTransactionIsolation(void) const;
	virtual void setTransactionIsolation(enum_transaction_isolation level)
			throw (SqlException&);

	virtual SqlStatement* createStatement(void) throw (SqlException&);
	virtual SqlPreparedStatement* preparedStatement(const std::string& sql)
			throw (SqlException&);

	virtual SqlTransaction* beginTransaction(void) throw (SqlException&);
	virtual uint64_t getInsertId(void) const;
private:
	uint32_t refCount_;
	time_t lastActiveTime_;
	long threadId_;
	SqlConnection* connection_;
};

class ThreadedDataSource: public DataSource
{
	friend class HeartbeatThread;
	friend class SqlConnectionProxy;
public:
	ThreadedDataSource(const DataSource::Config& config,
			SqlConnectionFactory* connectionFactory);

	virtual ~ThreadedDataSource(void);

	virtual std::auto_ptr<SqlConnection> getConnection(void)
			throw (SqlException&);
private:
	void returnConnection(DataSourceSqlConnection* connection);
	DataSourceSqlConnection* borrowConnection(long threadId);

	void init(void);
private:
	DataSource::Config config_;

	SqlConnectionFactory* connectionFactory_;

	mxcore::AutoMutexLock connectionLock_;
	mxcore::Condition connectionCond_;

	std::set<DataSourceSqlConnection*> connectionPool_;
	std::list<DataSourceSqlConnection*> freeConnectionPool_;

	std::map<long, DataSourceSqlConnection*> usedConnections_;

	mxcore::AutoMutexLock initLock_;
	HeartbeatThread* heartbeatThread_;
};

} // namespace mxsql

#endif /* __MXSQL_THREADEDDATASOURCE_H__ */
