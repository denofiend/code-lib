/*
 * LoggerFactory.cpp
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#include <iostream>
#include <mxcore/SystemOps.h>
#include <mxcore/Logger.h>
#include <mxcore/LoggerFactory.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

LoggerFactory::LoggerConfig::LoggerConfig(void) :
	level_(Logger::LOGLEVEL_INFO), bufferSize_(1024)
{
}

LoggerFactory::LoggerConfig::LoggerConfig(const LoggerConfig& src) :
	name_(src.name_), filepath_(src.filepath_), level_(src.level_),
			bufferSize_(src.bufferSize_)
{
}

LoggerFactory::LoggerConfig& LoggerFactory::LoggerConfig::operator =(
		const LoggerConfig& other)
{
	if (this != &other)
	{
		name_ = other.name_;
		filepath_ = other.filepath_;
		level_ = other.level_;
		bufferSize_ = other.bufferSize_;
	}
	return *this;
}

LoggerFactory::LoggerConfig::~LoggerConfig(void)
{
}

LoggerFactory::LoggerFactory(void)
{
}

LoggerFactory::~LoggerFactory(void)
{
	for (std::map<LoggerConfig, Logger*>::iterator it = namedLoggers_.begin(); it
			!= namedLoggers_.end(); it++)
	{
		delete it->second;
	}
	namedLoggers_.clear();
	filenamedLoggers_.clear();
}

LoggerFactory& LoggerFactory::getInstance(void)
{
	static LoggerFactory sInstance;
	return sInstance;
}

void LoggerFactory::reopen(void) throw (mxcore::IOException&)
{
	for (std::map<std::string, mxcore::SharedPtr<LoggerImpl> >::iterator it =
			filenamedLoggers_.begin(); it != filenamedLoggers_.end(); it++)
	{
		AbstractLoggerImpl* impl = (AbstractLoggerImpl*) it->second.pointer();
		impl->resetStream(stdout);
	}

	for (std::map<std::string, mxcore::SharedPtr<LoggerImpl> >::iterator it =
			filenamedLoggers_.begin(); it != filenamedLoggers_.end(); it++)
	{
		std::string filename = it->first;

		AbstractLoggerImpl* impl = (AbstractLoggerImpl*) it->second.pointer();

		if (filename.empty())
		{
			impl->resetStream(stdout);
		}
		else
		{
			FILE* stream = ::fopen(filename.c_str(), "a");

			if (NULL == stream)
			{
				THROW3(mxcore::IOException
						, std::string("Can't open file: [") + filename + "]"
						, mxos::getLastError());
			}

			impl->resetStream(stream);
		}
	}
}

Logger& LoggerFactory::createLogger(const LoggerConfig& config)
		throw (mxcore::IOException&)
{
	MX_ASSERT(!config.name_.empty());

	Logger* logger = findLoggerByName(config.name_);

	if (NULL != logger)
	{
		return *logger;
	}

	mxcore::SharedPtr<LoggerImpl> impl = findOrCreateImpl(config.filepath_);
	logger = new Logger(config.level_, config.name_, config.bufferSize_, impl);
	namedLoggers_.insert(std::make_pair(config, logger));

	return *logger;
}

Logger& LoggerFactory::getLogger(const std::string& name)
{
	Logger* logger = findLoggerByName(name);

	if (NULL == logger)
	{
		LoggerConfig config;
		config.name_ = name;
		config.bufferSize_ = DEFAULT_BUFFER_SIZE;
		config.level_ = Logger::LOGLEVEL_INFO;
		return createLogger(config);
	}

	return *logger;
}

Logger* LoggerFactory::findLoggerByName(const std::string& name) const
{
	LoggerConfig cfg;
	cfg.name_ = name;

	std::map<LoggerConfig, Logger*>::const_iterator it =
			namedLoggers_.find(cfg);

	if (it != namedLoggers_.end())
	{
		return it->second;
	}

	return NULL;
}

mxcore::SharedPtr<LoggerImpl> LoggerFactory::findLoggerByFilename(
		const std::string& filename) const
{
	std::map<std::string, mxcore::SharedPtr<LoggerImpl> >::const_iterator it =
			filenamedLoggers_.find(filename);

	if (it != filenamedLoggers_.end())
	{
		return it->second;
	}
	return mxcore::SharedPtr<LoggerImpl>();
}

mxcore::SharedPtr<LoggerImpl> LoggerFactory::findOrCreateImpl(
		const std::string& filename) throw (mxcore::IOException&)
{
	mxcore::SharedPtr<LoggerImpl> impl = findLoggerByFilename(filename);

	if (impl.isNull())
	{
		impl = createImpl(filename);
		filenamedLoggers_.insert(std::make_pair(filename, impl));
	}
	return impl;
}

mxcore::SharedPtr<LoggerImpl> LoggerFactory::createImpl(
		const std::string& filename) throw (mxcore::IOException&)
{
	mxcore::SharedPtr<LoggerImpl> impl;

	if (filename.empty())
	{
		impl = new LoggerImplT<mxcore::AutoMutexLock, false> (stdout);
	}
	else
	{
		FILE* stream = ::fopen(filename.c_str(), "a");

		if (NULL == stream)
		{
			THROW3(mxcore::IOException
					, std::string("Can't open file: [") + filename + "]"
					, mxos::getLastError());
		}

		impl = new LoggerImplT<mxcore::AutoMutexLock, true> (stream);
	}

	return impl;
}

} // nammespace mxcore
