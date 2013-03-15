/*
 *        Name: HttpConstants.cpp
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxhttp/HttpConstants.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

const std::string HttpConstants::CRLF("\r\n");

const std::string HttpConstants::HTTP_DEFAULT_VERSION("HTTP/1.1");

const std::string HttpConstants::HEADER_CONTENT_LENGTH("Content-Length");
const std::string HttpConstants::HEADER_CONTENT_TYPE("Content-Type");
const std::string HttpConstants::HEADER_CONNECTION("Connection");
const std::string HttpConstants::HEADER_PROXY_CONNECTION("Proxy-Connection");
const std::string HttpConstants::HEADER_TRANSFER_ENCODING("Transfer-Encoding");
const std::string HttpConstants::HEADER_DATE("Date");
const std::string HttpConstants::HEADER_SERVER("Server");
const std::string HttpConstants::HEADER_CONTENT_DISPOSITION(
		"Content-Disposition");
const std::string HttpConstants::HEADER_ACCEPT_ENCODING("Accept-Encoding");
const std::string HttpConstants::HEADER_USER_AGENT("User-Agent");
const std::string HttpConstants::HEADER_HOST("Host");
const std::string HttpConstants::HEADER_X_FORWARD_FOR("X-Forwarded-For");

const std::pair<int, std::string> HttpConstants::STATUS_OK = std::make_pair(
		200, "OK");

const std::pair<int, std::string> HttpConstants::STATUS_BAD_REQUEST =
		std::make_pair(400, "Bad Request");

const std::pair<int, std::string> HttpConstants::STATUS_NOT_FOUND =
		std::make_pair(404, "Not Found");

const std::pair<int, std::string> HttpConstants::STATUS_LENGTH_REQUIRED =
		std::make_pair(411, "Length Required");

const std::pair<int, std::string> HttpConstants::STATUS_ENTITY_TO0_LARGE =
		std::make_pair(413, "Request Entity Too Large");

const std::pair<int, std::string> HttpConstants::STATUS_METHOD_NOT_ALLOWED =
		std::make_pair(405, "Method Not Allowed");

const std::pair<int, std::string> HttpConstants::STATUS_SERVER_ERROR =
		std::make_pair(500, "Internal Server Error");

} //namespace mxhttp
