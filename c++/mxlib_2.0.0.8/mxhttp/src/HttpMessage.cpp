/*
 *        Name: HttpMessage.cpp
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxhttp/HttpMessage.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpMessage::HttpMessage(int type) :
		type_(type), version_(HttpConstants::HTTP_DEFAULT_VERSION), body_(NULL)
{
}

HttpMessage::~HttpMessage(void)
{
	if (NULL != body_)
	{
		delete body_;
		body_ = NULL;
	}
}

void HttpMessage::setBody(HttpBody* body)
{
	if (isResponse())
	{
		MX_ASSERT(NULL == body || !body->isMultipartBody());
	}

	if (NULL != body_)
	{
		delete body_;
	}
	body_ = body;
}

bool HttpMessage::isChunked(void) const
{
	static std::string sChunked("chunked");
	const std::string& encoding = getHeaders().getValue(
			HttpConstants::HEADER_TRANSFER_ENCODING);
	return mxcore::StringUtil::startWith(encoding, sChunked);
}

bool HttpMessage::isKeepAlive(void) const
{
	static const std::string sKeepAlive("Keep-Alive");
	const std::string& connection = getHeaders().getValue(
			HttpConstants::HEADER_CONNECTION);
	const std::string& proxyConnection = getHeaders().getValue(
			HttpConstants::HEADER_PROXY_CONNECTION);

	return (mxcore::StringUtil::equalNoCase(connection, sKeepAlive)
			|| mxcore::StringUtil::equalNoCase(proxyConnection, sKeepAlive));
}

const std::string HttpRequest::METHOD_GET("GET");
const std::string HttpRequest::METHOD_PUT("PUT");
const std::string HttpRequest::METHOD_POST("POST");
const std::string HttpRequest::METHOD_HEAD("HEAD");
const std::string HttpRequest::METHOD_DELETE("DELETE");

HttpRequest::HttpRequest(void) :
		HttpMessage(HttpMessage::eRequest)
{
}

HttpRequest::~HttpRequest(void)
{
}

HttpResponse::HttpResponse(void) :
		HttpMessage(HttpMessage::eResponse)
{
}

HttpResponse::~HttpResponse(void)
{
}

} // namespace mxhttp
