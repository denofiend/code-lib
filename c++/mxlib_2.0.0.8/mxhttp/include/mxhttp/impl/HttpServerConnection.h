/*
 * HttpServerConnection.h
 *
 *  Created on: 2011-3-1
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_IMPL_HTTPSERVERCONNECTION_H__
#define __MXHTTP_IMPL_HTTPSERVERCONNECTION_H__

#include <mxcore/SharedPtr.h>
#include <mxcore/ByteBuffer.h>

#include <mxhttp/HttpListening.h>
#include <mxhttp/HttpMsgParser.h>

namespace mxhttp
{

class HttpServerConnection
{
	friend class HttpServerImpl;
private:
	HttpServerConnection(const HttpServerConnection&);
	HttpServerConnection& operator =(const HttpServerConnection&);
public:
	HttpServerConnection(const HttpConnectionId& id, HttpListening* listening);
	~HttpServerConnection(void);
private:
	HttpConnectionId id_;
	HttpListening* listening_;

	HttpMsgParser* parser_;
	mxcore::SharedPtr<HttpRequest> request_;
	mxcore::SharedPtr<HttpResponse> response_;

	bool checkedUriAndHeaders_;
	bool canClosed_;

	mxcore::SharedPtr<mxcore::ByteBuffer> readBuf_;
};

} // namespace mxhttp

#endif /* __MXHTTP_IMPL_HTTPSERVERCONNECTION_H__ */
