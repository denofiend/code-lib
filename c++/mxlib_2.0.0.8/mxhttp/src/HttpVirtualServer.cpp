/*
 * HttpVirtualServer.cpp
 *
 *  Created on: 2011-3-3
 *      Author: zhaojiangang
 */

#include <mxcore/SystemOps.h>

#include <mxcore/Assert.h>
#include <mxcore/StringUtil.h>
#include <mxhttp/PcreUtil.h>
#include <mxhttp/HttpVirtualServer.h>
#include <mxhttp/HttpFileDownLoadServlet.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpVirtualServer::HttpVirtualServer(const std::string& host,
		const std::string& rootDir) :
	host_(host), rootDir_(rootDir), httpServer_(NULL)
{
	if (!rootDir_.empty() && mxcore::StringUtil::endWith(rootDir_, "/"))
	{
		rootDir_.erase(rootDir_.length() - 1);
	}
}

HttpVirtualServer::~HttpVirtualServer(void)
{
	for (PcreMap::iterator it = pcres_.begin(); it != pcres_.end(); it++)
	{
		mxhttp::PcreUtil::close(it->first);
	}
	pcres_.clear();
}

mxcore::SharedPtr<HttpServlet> HttpVirtualServer::findServlet(
		const HttpRequest& request) const
{
	ServletMap::const_iterator it = servlets_.find(request.getUri());

	if (it != servlets_.end())
	{
		return it->second;
	}

	for (PcreMap::const_iterator it = pcres_.begin(); it != pcres_.end(); it++)
	{
		if (0 == mxhttp::PcreUtil::match(it->first, request.getUri()))
		{
			return it->second;
		}
	}

	if (!filedownloadServlet_.isNull() && request.isGet())
	{
		return filedownloadServlet_;
	}

	return mxcore::SharedPtr<HttpServlet>(NULL);
}

bool HttpVirtualServer::addServlet(const std::string& path,
		mxcore::SharedPtr<HttpServlet> servlet)
{
	MX_ASSERT(!path.empty());
	MX_ASSERT(!servlet.isNull());

	ServletMap::const_iterator it = servlets_.find(path);

	if (it != servlets_.end())
	{
		return false;
	}

	pcre* re = mxhttp::PcreUtil::open(path);

	if (NULL == re)
	{
		return false;
	}

	servlets_.insert(std::make_pair(path, servlet));
	pcres_.insert(std::make_pair(re, servlet));

	return true;
}

std::string HttpVirtualServer::getAbsolutePath(const std::string& path) const
{
	return rootDir_ + path;
}

void HttpVirtualServer::setFiledownloadSetvlet(
		mxcore::SharedPtr<HttpServlet> filedownloadServlet)
{
	filedownloadServlet_ = filedownloadServlet;
}

mxcore::Logger& HttpVirtualServer::getLogger(void)
{
	if (loggerConfig_.name_.empty())
	{
		return mxcore::LoggerFactory::getInstance().getLogger("mxhttp");
	}

	return mxcore::LoggerFactory::getInstance().getLogger(loggerConfig_.name_);
}

} // namespace mxhttp
