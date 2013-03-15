/*
 * HttpServerConnection.cpp
 *
 *  Created on: 2011-3-1
 *      Author: zhaojiangang
 */

#include <mxhttp/impl/HttpServerConnection.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpServerConnection::HttpServerConnection(const HttpConnectionId& id,
		HttpListening* listening) :
	id_(id), listening_(listening), parser_(NULL),
			checkedUriAndHeaders_(false), canClosed_(false)
{
}

HttpServerConnection::~HttpServerConnection(void)
{
	if (NULL != parser_)
	{
		delete parser_;
		parser_ = NULL;
	}
}

} // namespace mxhttp
