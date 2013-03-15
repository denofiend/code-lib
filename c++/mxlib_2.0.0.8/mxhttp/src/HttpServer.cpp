/*
 * HttpServer.cpp
 *
 *  Created on: 2011-3-4
 *      Author: zhaojiangang
 */

#include <mxhttp/HttpServer.h>
#include <mxhttp/impl/HttpServerImpl.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpServer::HttpServer(void)
{
}

HttpServer::~HttpServer(void)
{
}

HttpServer* HttpServer::createInstance(const HttpConfig& config)
{
	return new HttpServerImpl(config);
}

} // namespace mxhttp
