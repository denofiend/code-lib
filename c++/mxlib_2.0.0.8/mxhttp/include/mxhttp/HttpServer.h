/*
 * HttpServer.h
 *
 *  Created on: 2011-3-4
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPSERVER_H__
#define __MXHTTP_HTTPSERVER_H__

#include <list>

#include <mxcore/Exception.h>
#include <mxcore/SocketAddress.h>

#include <mxasio/IOService.h>

#include <mxhttp/HttpConfig.h>
#include <mxhttp/HttpVirtualServer.h>

namespace mxhttp
{

class HttpServer
{
private:
	HttpServer(const HttpServer&);
	HttpServer& operator =(const HttpServer&);
public:
	HttpServer(void);
	virtual ~HttpServer(void);

	static HttpServer* createInstance(const HttpConfig& config);

	virtual void open(void) throw (mxcore::Exception&) = 0;

	virtual void reopen(void) throw (mxcore::Exception&) = 0;

	virtual void shutdown(void) throw (mxcore::Exception&) = 0;

	virtual void run(void) throw (mxcore::Exception&) = 0;

	virtual uint32_t getConnectionCount(void) const = 0;

	virtual bool addVirtualServer(
			mxcore::SharedPtr<HttpVirtualServer> virtualServer,
			const std::list<mxcore::SocketAddress>* bindList) = 0;

	virtual mxasio::IOService* getIOService(void) const = 0;
};

} // name

#endif /* HTTPSERVER_H_ */
