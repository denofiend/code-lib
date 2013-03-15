/*
 *        Name: HttpMessage.h
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPMESSAGE_H__
#define __MXHTTP_HTTPMESSAGE_H__

#include <string>
#include <vector>
#include <mxcore/Number.h>
#include <mxcore/StringUtil.h>

#include <mxhttp/HttpBody.h>
#include <mxhttp/HttpSimpleBody.h>
#include <mxhttp/HttpMultipartBody.h>

#include <mxhttp/HttpPairList.h>
#include <mxhttp/HttpConstants.h>

#include <mxhttp/HttpConnectionId.h>

namespace mxhttp
{

class HttpMessage
{
private:
	HttpMessage(const HttpMessage&);
	HttpMessage& operator =(const HttpMessage&);
protected:
	enum
	{
		eRequest, eResponse
	};

	HttpMessage(int type);
public:
	typedef std::pair<std::string, std::string> Header;

	virtual ~HttpMessage(void);

	bool isRequest(void) const;
	bool isResponse(void) const;

	bool isChunked(void) const;
	bool isKeepAlive(void) const;

	void setVersion(const std::string& version);
	const std::string& getVersion(void) const;

	HttpBody* getBody(void) const;
	void setBody(HttpBody* body);

	const HttpConnectionId& getConnectionId(void) const;
	void setConnectionId(const HttpConnectionId& connectionId);

	HttpHeaderList& getHeaders(void);
	const HttpHeaderList& getHeaders(void) const;
private:
	int type_;
	std::string version_;
	HttpHeaderList headers_;
	HttpBody* body_;

	HttpConnectionId connectionId_;
};

class HttpRequest: public HttpMessage
{
private:
	HttpRequest(const HttpRequest&);
	HttpRequest& operator =(const HttpRequest&);
public:
	static const std::string METHOD_GET;
	static const std::string METHOD_PUT;
	static const std::string METHOD_POST;
	static const std::string METHOD_HEAD;
	static const std::string METHOD_DELETE;

	typedef std::pair<std::string, std::string> Param;

	HttpRequest(void);
	virtual ~HttpRequest(void);

	void setMethod(const std::string& method);
	const std::string& getMethod(void) const;

	void setUri(const std::string& uri);
	const std::string& getUri(void) const;

	bool isGet(void) const;
	bool isPut(void) const;
	bool isPost(void) const;
	bool isHead(void) const;
	bool isDelete(void) const;

	bool isMultiPart(void) const;
	bool isFormUrlEncoded(void) const;

	std::string getBoundary(void) const;

	HttpParamList& getParams(void);
	const HttpParamList& getParams(void) const;

	bool isKeepAlive(void) const;
	const std::string& getClientIp(void) const;
	void setClientIp(const std::string& clientIp);
private:
	std::string method_;
	std::string uri_;
	std::string clientIp_;
	HttpParamList params_;
};

class HttpResponse: public HttpMessage
{
private:
	HttpResponse(const HttpResponse&);
	HttpResponse& operator =(const HttpResponse&);
public:
	HttpResponse(void);
	virtual ~HttpResponse(void);

	void setStatusCode(int statusCode);
	int getStatusCode(void) const;

	void setReasonPhrase(const std::string& reasonPhrase);
	const std::string& getReasonPhrase(void) const;
private:
	int statusCode_;
	std::string reasonPhrase_;
};

////////////////////////////////////////////////////
// http getMessage

inline bool HttpMessage::isRequest(void) const
{
	return eRequest == type_;
}

inline bool HttpMessage::isResponse(void) const
{
	return eResponse == type_;
}

inline void HttpMessage::setVersion(const std::string& version)
{
	if (version.empty())
	{
		version_ = HttpConstants::HTTP_DEFAULT_VERSION;
	}
	else
	{
		version_ = version;
	}
}

inline const std::string& HttpMessage::getVersion(void) const
{
	return version_;
}

inline HttpBody* HttpMessage::getBody(void) const
{
	return body_;
}

inline const HttpConnectionId& HttpMessage::getConnectionId(void) const
{
	return connectionId_;
}

inline void HttpMessage::setConnectionId(const HttpConnectionId& connectionId)
{
	connectionId_ = connectionId;
}

inline HttpHeaderList& HttpMessage::getHeaders(void)
{
	return headers_;
}

inline const HttpHeaderList& HttpMessage::getHeaders(void) const
{
	return headers_;
}

//////////////////////////////////////////////////////
// http request

inline void HttpRequest::setMethod(const std::string& method)
{
	method_ = method;
}

inline const std::string& HttpRequest::getMethod(void) const
{
	return method_;
}

inline void HttpRequest::setUri(const std::string& uri)
{
	uri_ = uri;
}

inline const std::string& HttpRequest::getUri(void) const
{
	return uri_;
}

inline bool HttpRequest::isGet(void) const
{
	return mxcore::StringUtil::equalNoCase(METHOD_GET, method_);
}

inline bool HttpRequest::isPut(void) const
{
	return mxcore::StringUtil::equalNoCase(METHOD_PUT, method_);
}

inline bool HttpRequest::isPost(void) const
{
	return mxcore::StringUtil::equalNoCase(METHOD_POST, method_);
}

inline bool HttpRequest::isHead(void) const
{
	return mxcore::StringUtil::equalNoCase(METHOD_HEAD, method_);
}

inline bool HttpRequest::isDelete(void) const
{
	return mxcore::StringUtil::equalNoCase(METHOD_DELETE, method_);
}

inline bool HttpRequest::isMultiPart(void) const
{
	const std::string& contentType = getHeaders().getValue(
			HttpConstants::HEADER_CONTENT_TYPE);

	return std::string::npos != contentType.find("multipart/form-data");
}

inline bool HttpRequest::isFormUrlEncoded(void) const
{
	static const std::string sFormUrlEncoded(
			"application/x-www-form-urlencoded");

	const std::string& contentType = getHeaders().getValue(
			HttpConstants::HEADER_CONTENT_TYPE);

	return mxcore::StringUtil::startWith(contentType, sFormUrlEncoded);
}

inline std::string HttpRequest::getBoundary(void) const
{
	const std::string& contentType = getHeaders().getValue(
			HttpConstants::HEADER_CONTENT_TYPE);

	size_t pos = contentType.find("multipart/form-data");

	if (std::string::npos != pos)
	{
		pos = contentType.find("boundary=");

		if (std::string::npos != pos && (pos + 9 < contentType.length()))
		{
			return contentType.substr(pos + 9);
		}
	}

	return "";
}

inline HttpParamList& HttpRequest::getParams(void)
{
	return params_;
}

inline const HttpParamList& HttpRequest::getParams(void) const
{
	return params_;
}

inline bool HttpRequest::isKeepAlive(void) const
{
	static const std::string sKeepAlive("Keep-Alive");

	return mxcore::StringUtil::equalNoCase(sKeepAlive,
			getHeaders().getValue(HttpConstants::HEADER_CONNECTION))
			|| mxcore::StringUtil::equalNoCase(
					sKeepAlive,
					getHeaders().getValue(
							HttpConstants::HEADER_PROXY_CONNECTION));
}

inline const std::string& HttpRequest::getClientIp(void) const
{
	return clientIp_;
}

inline void HttpRequest::setClientIp(const std::string& clientIp)
{
	clientIp_ = clientIp;
}

inline void HttpResponse::setStatusCode(int statusCode)
{
	statusCode_ = statusCode;
}

inline int HttpResponse::getStatusCode(void) const
{
	return statusCode_;
}

inline void HttpResponse::setReasonPhrase(const std::string& reasonPhrase)
{
	reasonPhrase_ = reasonPhrase;
}

inline const std::string& HttpResponse::getReasonPhrase(void) const
{
	return reasonPhrase_;
}

} // namespace mxhttp

#endif /* __MXHTTP_HTTPMESSAGE_H__ */
