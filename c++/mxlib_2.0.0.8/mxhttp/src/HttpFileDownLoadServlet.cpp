/*
 * HttpFileDownLoadServlet.cpp
 *
 *  Created on: 2011-3-8
 *      Author: zhaojiangang
 */

#include <iostream>
#include <mxhttp/HttpUtil.h>
#include <mxcore/LoggerFactory.h>
#include <mxhttp/HttpVirtualServer.h>
#include <mxhttp/HttpFileDownLoadServlet.h>

#include <mxcore/SystemOps.h>
#include <mxcore/StringUtil.h>
#include <mxcore/GenernicFactory.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpFileDownLoadServlet::HttpFileDownLoadServlet(void)
{
}

HttpFileDownLoadServlet::~HttpFileDownLoadServlet(void)
{
}

void HttpFileDownLoadServlet::service(mxcore::SharedPtr<HttpVirtualServer> vs,
		mxcore::SharedPtr<HttpRequest> request, HttpResponser* responser)
{
	if (!request->isGet())
	{
		responser->response(
				request,
				HttpUtil::makeResponse(*request,
						HttpConstants::STATUS_METHOD_NOT_ALLOWED, NULL));
	}
	else
	{
		std::string uri = request->getUri();

		std::string filePath = vs->getAbsolutePath(uri);

		if (!canAccess(uri))
		{
			responser->response(
					request,
					HttpUtil::makeResponse(*request,
							HttpConstants::STATUS_NOT_FOUND, NULL));

			vs->getLogger().debug("Connection: [%s] try to access file [%s]\n",
					request->getConnectionId().toString().c_str(),
					filePath.c_str());

			return;
		}

		responser->response(request,
				HttpUtil::makeFileResponse(*request, filePath, false));
	}
}

void HttpFileDownLoadServlet::initImpl(void) throw (mxcore::Exception&)
{
	std::map<std::string, std::vector<std::string> >::const_iterator it =
			params_.find("allow");

	if (it != params_.end())
	{
		addFilters(allowList_, it->second);
	}

	it = params_.find("denine");

	if (it != params_.end())
	{
		addFilters(denineList_, it->second);
	}
}

bool HttpFileDownLoadServlet::doFilter(const std::string& uri,
		std::list<pcre*>& filterList)
{
	for (std::list<pcre*>::iterator it = filterList.begin(); it
			!= filterList.end(); it++)
	{
		if (0 == mxhttp::PcreUtil::match(*it, uri))
		{
			return true;
		}
	}

	return false;
}

bool HttpFileDownLoadServlet::canAccess(const std::string& uri)
{
	if (doFilter(uri, denineList_))
	{
		return false;
	}

	return doFilter(uri, allowList_);
}

void HttpFileDownLoadServlet::addFilters(std::list<pcre*>& li,
		const std::vector<std::string>& params) throw (mxcore::Exception&)
{
	for (std::vector<std::string>::const_iterator it = params.begin(); it
			!= params.end(); it++)
	{
		if (!(*it).empty())
		{
			pcre* re = mxhttp::PcreUtil::open(*it);

			if (NULL == re)
			{
				THROW2(mxcore::Exception
						, std::string("Can't compile pattern: [") + *it + "]");
			}
			li.push_back(re);
		}
	}
}

} // namespace mxhttp
