/*
 * Logger.h
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#ifndef __MXLIB_LOGGER_H__
#define __MXLIB_LOGGER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <mxcore/Lock.h>
#include <mxcore/SharedPtr.h>

namespace mxcore
{

class LoggerImpl
{
public:
	virtual ~LoggerImpl(void)
	{
	}

	virtual void doLog(const char* buffer, size_t len) = 0;
};

class Logger
{
	friend class LoggerFactory;
	Logger(const Logger&);
	Logger& operator =(const Logger&);
public:
	typedef enum
	{
		LOGLEVEL_OFF = 0,
		LOGLEVEL_ERROR = 1,
		LOGLEVEL_WARNING = 2,
		LOGLEVEL_INFO = 3,
		LOGLEVEL_DEBUG = 4,
		LOGLEVEL_TRACE = 5
	} LogLevel;

	Logger(int level, const std::string& name, uint16_t bufferSize,
			mxcore::SharedPtr<LoggerImpl> impl);
	~Logger(void);

	void trace(const char* fmt, ...);
	void debug(const char* fmt, ...);
	void info(const char* fmt, ...);
	void infoOk(const char* fmt, ...);
	void infoFail(const char* fmt, ...);
	void infoFail1(const char* fmt, ...);
	void warn(const char* fmt, ...);
	void error(const char* fmt, ...);
	void error1(const char* fmt, ...);
	void error2(int errNo, const char* fmt, ...);
	void log(int level, const char* fmt, ...);
	void log1(int level, const char* fmt, ...);

	bool isTraceEnable(void) const;
	bool isDebugEnable(void) const;
	bool isInfoEnable(void) const;
	bool isWarnEnable(void) const;
	bool isErrorEnable(void) const;
	bool isLevelEnable(int level) const;
private:
	void logImpl(int errNo, int level, const char* levelName, const char* fmt,
			va_list vp);
private:
	int level_;
	std::string name_;

	char* buffer_;
	uint16_t bufferSize_;

	mxcore::SharedPtr<LoggerImpl> impl_;
};

inline bool Logger::isTraceEnable(void) const
{
	return isLevelEnable(LOGLEVEL_TRACE);
}

inline bool Logger::isDebugEnable(void) const
{
	return isLevelEnable(LOGLEVEL_DEBUG);
}

inline bool Logger::isInfoEnable(void) const
{
	return isLevelEnable(LOGLEVEL_INFO);
}

inline bool Logger::isWarnEnable(void) const
{
	return isLevelEnable(LOGLEVEL_WARNING);
}

inline bool Logger::isErrorEnable(void) const
{
	return isLevelEnable(LOGLEVEL_ERROR);
}

inline bool Logger::isLevelEnable(int level) const
{
	return level_ >= level;
}

} // namespace mxcore

#endif /* __MXLIB_LOGGER_H__ */
