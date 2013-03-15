/*
 * HttpListening.cpp
 *
 *  Created on: 2011-3-3
 *      Author: zhaojiangang
 */

#include <mxcore/Assert.h>
#include <mxcore/StringUtil.h>
#include <mxhttp/HttpListening.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpListening::HttpListening(const mxcore::SocketAddress& bindAddress) :
	bindAddress_(bindAddress)
{
}

HttpListening::~HttpListening(void)
{
}

bool HttpListening::addVirtualServer(
		mxcore::SharedPtr<HttpVirtualServer> virtualServer)
{
	MX_ASSERT(!virtualServer.isNull());

	if (isDomainExists(virtualServer->getHost()))
	{
		return false;
	}

	virtualServers_.push_back(virtualServer);
	return true;
}

static std::string getDomain(const std::string& host)
{
	size_t pos = host.find(':');

	if (std::string::npos == pos)
	{
		return host;
	}

	return host.substr(0, pos);
}

bool HttpListening::findServlet(
		const HttpRequest& request,
		std::pair<mxcore::SharedPtr<HttpVirtualServer>, mxcore::SharedPtr<
				HttpServlet> >& result) const
{
	std::string host =
			request.getHeaders().getValue(HttpConstants::HEADER_HOST);

	std::string domain = getDomain(host);

	mxcore::SharedPtr<HttpVirtualServer> vs = findVirtualServer(domain);

	if (vs.isNull())
	{
		vs = getFirstVirtualServer();
	}

	if (vs.isNull())
	{
		return false;
	}

	mxcore::SharedPtr<HttpServlet> servlet = vs->findServlet(request);

	if (servlet.isNull())
	{
		return false;
	}

	result.first = vs;
	result.second = servlet;
	return true;
}

bool HttpListening::isDomainExists(const std::string& domain) const
{
	mxcore::SharedPtr<HttpVirtualServer> vs = findVirtualServer(domain);
	return !vs.isNull();
}

mxcore::SharedPtr<HttpVirtualServer> HttpListening::getFirstVirtualServer(void) const
{
	mxcore::SharedPtr<HttpVirtualServer> vs;

	if (!virtualServers_.empty())
	{
		vs = virtualServers_.front();
	}

	return vs;
}

mxcore::SharedPtr<HttpVirtualServer> HttpListening::findVirtualServer(
		const std::string& domain) const
{
	for (VirtualServerList::const_iterator it = virtualServers_.begin(); it
			!= virtualServers_.end(); it++)
	{
		const mxcore::SharedPtr<HttpVirtualServer>& vs = *it;

		if (mxcore::StringUtil::equalNoCase(domain, vs->getHost()))
		{
			return vs;
		}
	}

	static mxcore::SharedPtr<HttpVirtualServer> nullVs;
	return nullVs;
}

} // namespace mxhttp
