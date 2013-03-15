/*
 * Logger.cpp
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#include <string>
#include <mxcore/SystemOps.h>

#include <mxcore/Number.h>
#include <mxcore/Assert.h>
#include <mxcore/Logger.h>
#include <mxcore/TimeUtil.h>
#include <mxcore/MutexLock.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

static const char* LEVEL_NAMES[] =
{ "OFF", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE" };

inline static const char* getLevelName(int level)
{
	if (level < 0 || level > Logger::LOGLEVEL_TRACE)
	{
		return NULL;
	}

	return LEVEL_NAMES[level];
}

Logger::Logger(int level, const std::string& name, uint16_t bufferSize,
		mxcore::SharedPtr<LoggerImpl> impl) :
	level_(level), name_(name), buffer_(NULL), bufferSize_(bufferSize),
			impl_(impl)
{
	MX_ASSERT(bufferSize_ > 0);
	buffer_ = new char[bufferSize_ + 1];
}

Logger::~Logger(void)
{
	if (NULL != buffer_)
	{
		delete[] buffer_;
		buffer_ = NULL;
	}
}

void Logger::trace(const char* fmt, ...)
{
	if (isTraceEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_TRACE, "TRACE", fmt, ap);
		va_end(ap);
	}
}

void Logger::debug(const char* fmt, ...)
{
	if (isDebugEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_DEBUG, "DEBUG", fmt, ap);
		va_end(ap);
	}
}

void Logger::info(const char* fmt, ...)
{
	if (isInfoEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_INFO, "INFO", fmt, ap);
		va_end(ap);
	}
}

void Logger::infoOk(const char* fmt, ...)
{
	if (isInfoEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_INFO, "OK", fmt, ap);
		va_end(ap);
	}
}

void Logger::infoFail(const char* fmt, ...)
{
	if (isInfoEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_INFO, "FAIL", fmt, ap);
		va_end(ap);
	}
}

void Logger::infoFail1(const char* fmt, ...)
{
	if (isInfoEnable())
	{
		int err = mxos::getLastError();
		va_list ap;
		va_start(ap, fmt);
		logImpl(err, LOGLEVEL_INFO, "FAIL", fmt, ap);
		va_end(ap);
	}
}

void Logger::warn(const char* fmt, ...)
{
	if (isWarnEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_WARNING, "WARN", fmt, ap);
		va_end(ap);
	}
}

void Logger::error(const char* fmt, ...)
{
	if (isErrorEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_ERROR, "ERROR", fmt, ap);
		va_end(ap);
	}
}

void Logger::error1(const char* fmt, ...)
{
	if (isErrorEnable())
	{
		int err = mxos::getLastError();
		va_list ap;
		va_start(ap, fmt);
		logImpl(err, LOGLEVEL_ERROR, "ERROR", fmt, ap);
		va_end(ap);
	}
}

void Logger::error2(int errNo, const char* fmt, ...)
{
	if (isErrorEnable())
	{
		va_list ap;
		va_start(ap, fmt);
		logImpl(errNo, LOGLEVEL_ERROR, "ERROR", fmt, ap);
		va_end(ap);
	}
}

void Logger::log(int level, const char* fmt, ...)
{
	if (isLevelEnable(level))
	{
		std::string levelname;

		if (level >= 0 && level <= LOGLEVEL_TRACE)
		{
			levelname = LEVEL_NAMES[level];
		}
		else
		{
			levelname = mxcore::Integer(level).toString();
		}

		va_list ap;
		va_start(ap, fmt);
		logImpl(0, LOGLEVEL_ERROR, levelname.c_str(), fmt, ap);
		va_end(ap);
	}
}

void Logger::log1(int level, const char* fmt, ...)
{
	if (isLevelEnable(level))
	{
		std::string levelname;
		int err = mxos::getLastError();

		if (level >= 0 && level <= LOGLEVEL_TRACE)
		{
			levelname = LEVEL_NAMES[level];
		}
		else
		{
			levelname = mxcore::Integer(level).toString();
		}

		va_list ap;
		va_start(ap, fmt);
		logImpl(err, LOGLEVEL_ERROR, levelname.c_str(), fmt, ap);
		va_end(ap);
	}
}

void Logger::logImpl(int errNo, int level, const char* levelName,
		const char* fmt, va_list vp)
{
	size_t pos = 0;

	time_t t = time(NULL);
	struct tm tTm = TimeUtil::getLocalTime(t);
	pos = ::strftime(buffer_, bufferSize_ - pos, "%Y-%m-%d %H:%M:%S", &tTm);

	pos += ::snprintf(buffer_ + pos, bufferSize_ - pos, " [%d] ",
			(int) mxos::getPid());

	if (pos < bufferSize_)
	{
		pos += ::snprintf(buffer_ + pos, bufferSize_ - pos, "[%s] ",
				name_.c_str());
	}

	if (pos < bufferSize_)
	{
		pos
				+= ::snprintf(buffer_ + pos, bufferSize_ - pos, "[%s]: ",
						levelName);
	}

	if (errNo != 0 && pos < bufferSize_)
	{
		pos += ::snprintf(buffer_ + pos, bufferSize_ - pos, "%d:%s!: ", errNo,
				mxos::getErrorMsg(errNo));
	}

	if (pos < bufferSize_)
	{
		pos += ::vsnprintf(buffer_ + pos, bufferSize_ - pos, fmt, vp);
	}

	if (pos >= bufferSize_)
	{
		pos = bufferSize_;
	}

	buffer_[pos] = '\0';
	impl_->doLog(buffer_, pos);
}

} // namespace mxcore
