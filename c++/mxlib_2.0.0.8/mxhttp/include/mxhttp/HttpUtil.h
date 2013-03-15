/*
 *        Name: HttpUtil.h
 *
 *  Created on: 2011-2-24
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPUTIL_H__
#define __MXHTTP_HTTPUTIL_H__

#include <string>
#include <mxhttp/HttpPairList.h>
#include <mxhttp/HttpMessage.h>
#include <mxhttp/HttpConstants.h>

namespace mxhttp
{

class HttpUtil
{
private:
	HttpUtil(void);
public:
	static HttpResponse* makeResponse(const HttpRequest& request,
			const std::pair<int, std::string>& status, const std::string& body,
			const std::string& contentType);

	static HttpResponse* makeResponse(const HttpRequest& request,
			int statusCode, const std::string& reasonPhrase,
			const std::string& body, const std::string& contentType);

	static HttpResponse* makeResponse(const HttpRequest& request,
			int statusCode, const std::string& reasonPhrase, HttpBody* body);

	static HttpResponse* makeResponse(const HttpRequest& request,
			const std::pair<int, std::string>& status, HttpBody* body);

	static void makeResponse(const HttpRequest& request,
			HttpResponse& response, int statusCode,
			const std::string& reasonPhrase, HttpBody* body);

	static void makeResponse(const HttpRequest& request,
			HttpResponse& response, const std::pair<int, std::string>& status,
			HttpBody* body);

	static HttpResponse* makeFileResponse(const HttpRequest& request,
			const std::string& filepath, bool isTemp);

	static void encodeRequestHeaders(HttpRequest& request, std::string& str);
	static void encodeResponseHeaders(HttpResponse& response, std::string& str);

	static void encodeParams(const HttpParamList& params, std::string& str);
	static void encodeHeaders(const HttpHeaderList& headers, std::string& str);

	static bool encodeStartLine(HttpRequest& request, std::string& str);
	static void encodeStartLine(HttpResponse& response, std::string& str);

	static int64_t buildContentLength(const HttpMultipartBody* body);
	static const std::string& getFileContentType(const std::string& filepath);

	static void writeToFile(HttpUploadFileItem* item,
			const std::string& filename) throw (mxcore::IOException&);
	static void writeToFile(HttpSimpleBody* body, const std::string& filename)
			throw (mxcore::IOException&);
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPUTIL_H__ */
