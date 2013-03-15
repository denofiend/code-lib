/*
 * HttpListening.h
 *
 *  Created on: 2011-3-3
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPLISTENING_H__
#define __MXHTTP_HTTPLISTENING_H__

#include <list>
#include <mxcore/SocketAddress.h>
#include <mxhttp/HttpVirtualServer.h>

namespace mxhttp
{

class HttpListening
{
public:
	HttpListening(const mxcore::SocketAddress& bindAddress);
	~HttpListening(void);

	bool addVirtualServer(mxcore::SharedPtr<HttpVirtualServer> virtualServer);

	bool findServlet(
			const HttpRequest& request,
			std::pair<mxcore::SharedPtr<HttpVirtualServer>, mxcore::SharedPtr<
					HttpServlet> >& result) const;

	const mxcore::SocketAddress& getBindAddress(void) const;
private:
	bool isDomainExists(const std::string& host) const;

	mxcore::SharedPtr<HttpVirtualServer> findVirtualServer(const std::string& domain) const;

	mxcore::SharedPtr<HttpVirtualServer> getFirstVirtualServer(void) const;
private:
	mxcore::SocketAddress bindAddress_;
	typedef std::list< mxcore::SharedPtr<HttpVirtualServer> > VirtualServerList;
	VirtualServerList virtualServers_;
};

inline const mxcore::SocketAddress& HttpListening::getBindAddress(void) const
{
	return bindAddress_;
}

} // namespace mxhttp

#endif /* __MXHTTP_HTTPLISTENING_H__ */
