/*
 *        Name: main.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string>
#include <mxcore/MemRecord.h>
#include <iostream>
#include <mxcore/Real.h>
#include <mxcore/Number.h>
#include <mxcore/ScopePtr.h>
#include <mxcore/SharedPtr.h>
#include <mxsql/MysqlTest.h>
#include <mxcore/SocketTest.h>
#include <mxcore/SocketAddressTest.h>
#include <mxhttp/HttpBodyTest.h>
#include <mxhttp/HttpParserTest.h>
#include <mxcore/BytesUtilTest.h>
#include <mxasio/IOServiceTest.h>
#include <mxhttp/HttpServerTest.h>
#include <mxhttp/HttpVirtualServerTest.h>
#include <mxcore/LoggerFactory.h>
#include <mxsql/DataSourceTest.h>
#include <mxcore/MemRecord.h>

using namespace std;

#ifdef TEST

int main(int argc, char* argv[])
{
	//	mxcore::LoggerFactory::LoggerConfig logConfig;
	//	logConfig.bufferSize_ = 1024;
	//	logConfig.filepath_ = "./log.txt";
	//	logConfig.level_ = 100;
	//	logConfig.name_ = "hello";
	//
	//	try
	//	{
	//		mxcore::LoggerFactory::getInstance().createLogger(logConfig);
	//		logConfig.name_ = "hello2";
	//		logConfig.filepath_ = "./hello2.txt";
	//		mxcore::LoggerFactory::getInstance().createLogger(logConfig);
	//	} catch (mxcore::IOException& e)
	//	{
	//		std::cout << e.name() << ", " << e.getMessage() << ", "
	//				<< e.getFilename() << ":" << e.getLineNumber() << std::endl;
	//	}
	//
	//	mxcore::Logger& helloLog = mxcore::LoggerFactory::getInstance().getLogger(
	//			"hello");
	//	helloLog.error("hello\n");
	//
	//	mxcore::Logger& hello2Log = mxcore::LoggerFactory::getInstance().getLogger(
	//			"hello2");
	//	hello2Log.debug("this is debug\n");
	//
	//	helloLog.info("this is info\n");
	//	return 0;
	//	{
	//		mxsql::MysqlTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
	//	{
	//		mxcore::SocketAddressTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
	//	{
	//		mxcore::BytesUtilTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
	//	{
	//		mxcore::SocketTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
	//	{
	//		mxhttp::HttpBodyTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
	//	{
	//		mxhttp::HttpParserTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
	//	{
	//		mxasio::IOServiceTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
	//	{
	//
	//		mxhttp::HttpVirtualServerTest t;
	//		t.init();
	//		t.perform();
	//		t.cleanup();
	//	}
//	{
//		mxhttp::HttpServerTest t;
//		t.init();
//		t.perform();
//		t.cleanup();
//	}
	{
		mxsql::DataSourceTest t;
		t.init();
		t.perform();
		t.cleanup();
	}
	return 0;
}
#endif
