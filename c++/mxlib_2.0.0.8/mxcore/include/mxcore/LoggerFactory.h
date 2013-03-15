/*
 * LoggerFactory.h
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#ifndef __MXCORE_LOGGERFACTORY_H__
#define __MXCORE_LOGGERFACTORY_H__

#include <map>
#include <mxcore/Logger.h>
#include <mxcore/MutexLock.h>
#include <mxcore/IOException.h>
#include <mxcore/SharedPtr.h>

namespace mxcore
{

class AbstractLoggerImpl: public LoggerImpl
{
public:
	virtual ~AbstractLoggerImpl(void)
	{
	}

	virtual void resetStream(FILE* stream) = 0;
};

template<class LockT, bool isManagerStream>
class LoggerImplT: public AbstractLoggerImpl
{
	friend class LoggerFactory;
public:
	LoggerImplT(FILE* stream) :
		stream_(stream)
	{
		MX_ASSERT(NULL != stream_);
	}

	~LoggerImplT(void)
	{
		if (NULL != stream_ && isManagerStream)
		{
			::fclose(stream_);
			stream_ = NULL;
		}
	}

	virtual void resetStream(FILE* stream)
	{
		MX_ASSERT(NULL != stream);

		if (NULL != stream_ && isManagerStream && stream_ != stdout)
		{
			::fclose(stream_);
		}
		stream_ = stream;
	}

	virtual void doLog(const char* buffer, size_t len)
	{
		MX_ASSERT(NULL != buffer && len > 0);
		mxcore::Lock::ScopeLock sl(lock_);
		::fwrite(buffer, len, 1, stream_);
		::fflush(stream_);
	}
private:
	FILE* stream_;
	LockT lock_;
};

class LoggerFactory
{
private:
	LoggerFactory(void);
public:
	static const int DEFAULT_BUFFER_SIZE = 1024;

	class LoggerConfig
	{
	public:
		LoggerConfig(void);
		LoggerConfig(const LoggerConfig& src);
		LoggerConfig& operator =(const LoggerConfig& other);
		~LoggerConfig(void);

		friend bool operator <(const LoggerConfig& lc1, const LoggerConfig& lc2)
		{
			return lc1.name_ < lc2.name_;
		}
	public:
		std::string name_;
		std::string filepath_;
		int level_;
		uint16_t bufferSize_;
	};

	~LoggerFactory(void);

	static LoggerFactory& getInstance(void);

	void reopen(void) throw (mxcore::IOException&);

	Logger& createLogger(const LoggerConfig& config)
			throw (mxcore::IOException&);

	Logger& getLogger(const std::string& name);
private:
	Logger* findLoggerByName(const std::string& name) const;
	mxcore::SharedPtr<LoggerImpl>
	findLoggerByFilename(const std::string& filename) const;

	mxcore::SharedPtr<LoggerImpl> findOrCreateImpl(const std::string& filename)
			throw (mxcore::IOException&);
	mxcore::SharedPtr<LoggerImpl> createImpl(const std::string& filename)
			throw (mxcore::IOException&);
private:
	mxcore::AutoMutexLock loggersLock_;
	std::map<LoggerConfig, Logger*> namedLoggers_;
	std::map<std::string, mxcore::SharedPtr<LoggerImpl> > filenamedLoggers_;
};

} // namespace mxcore

#endif /* __MXCORE_LOGGERFACTORY_H__ */
