/*
 * main.cpp
 *
 *  Created on: 2011-3-17
 *      Author: zhaojiangang
 */

#include <mxcore/SystemOps.h>
#include <mxcore/Thread.h>
#include <mxcore/ScopePtr.h>
#include <mxhttp/HttpServer.h>
#include <mxhttp/HttpConfigurationXml.h>
#include <mxcore/LoggerFactory.h>
#include <mxcore/MemRecord.h>

#ifndef TEST

static bool getHomeDir(char* argv[], std::string& homeDir)
{
	homeDir = argv[0];
	size_t pos = homeDir.find_last_of('/');
	if (std::string::npos == pos)
	{
		return false;
	}

	homeDir.erase(pos + 1);
	return true;
}

int main(int argc, char* argv[])
{
	std::string homeDir;

	if (!getHomeDir(argv, homeDir))
	{
		return -1;
	}

	try
	{
		mxhttp::HttpConfigurationXml configuration(
				homeDir + "../conf/mxhttpserv.xml");
		mxcore::ScopePtr<mxhttp::HttpServer> server(
				configuration.buildInstance());
		server->open();
		mxos::daemonInit(argv[0], "", 0);
		mxcore::Thread::sleep(2000);
		server->reopen();
		server->run();
		return 0;
	} catch (mxcore::Exception& e)
	{
		mxcore::LoggerFactory::getInstance().getLogger("mxhttp").error2(
				e.getErrorCode(), "%s, %s, %s:%d\n", e.name().c_str(),
				e.getMessage().c_str(), e.getFilename().c_str(),
				e.getLineNumber());
		return -1;
	}
}

#endif
