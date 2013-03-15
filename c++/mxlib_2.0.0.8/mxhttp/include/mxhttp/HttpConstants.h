/*
 *        Name: HttpConstants.h
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPCONSTANTS_H__
#define __MXHTTP_HTTPCONSTANTS_H__

#include <string>

namespace mxhttp
{

class HttpConstants
{
private:
	HttpConstants(void);
public:
	static const std::string CRLF;

	static const std::string HTTP_DEFAULT_VERSION;

	static const std::string HEADER_CONTENT_LENGTH;
	static const std::string HEADER_CONTENT_TYPE;
	static const std::string HEADER_CONNECTION;
	static const std::string HEADER_PROXY_CONNECTION;
	static const std::string HEADER_TRANSFER_ENCODING;
	static const std::string HEADER_DATE;
	static const std::string HEADER_SERVER;
	static const std::string HEADER_CONTENT_DISPOSITION;
	static const std::string HEADER_ACCEPT_ENCODING;
	static const std::string HEADER_USER_AGENT;
	static const std::string HEADER_HOST;
	static const std::string HEADER_X_FORWARD_FOR;

	static const std::pair<int, std::string> STATUS_OK;
	static const std::pair<int, std::string> STATUS_BAD_REQUEST;
	static const std::pair<int, std::string> STATUS_NOT_FOUND;
	static const std::pair<int, std::string> STATUS_ENTITY_TO0_LARGE;
	static const std::pair<int, std::string> STATUS_LENGTH_REQUIRED;
	static const std::pair<int, std::string> STATUS_SERVER_ERROR;
	static const std::pair<int, std::string> STATUS_METHOD_NOT_ALLOWED;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPCONSTANTS_H__ */
