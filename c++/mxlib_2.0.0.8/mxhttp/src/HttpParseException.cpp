/*
 *        Name: HttpParseException.cpp
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxhttp/HttpConstants.h>
#include <mxhttp/HttpParseException.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpParseException::HttpParseException(const std::string& message,
		int errorCode, const std::string& filename, int line) :
	HttpException(message, errorCode, filename, line,
			HttpConstants::STATUS_BAD_REQUEST)
{
}

HttpParseException::HttpParseException(const std::string& message,
		int errorCode, const std::string& filename, int line, const std::pair<
				int, std::string>& httpStatus) :
	HttpException(message, errorCode, filename, line, httpStatus)
{
}

HttpParseException::HttpParseException(const HttpParseException& src) :
	HttpException(src)
{
}

HttpParseException::~HttpParseException(void)
{
}

HttpParseException& HttpParseException::operator =(
		const HttpParseException& other)
{
	HttpException::operator=(other);
	return *this;
}

const std::string& HttpParseException::name(void) const
{
	static const std::string sName("mxhttp::HttpParseException");
	return sName;
}

} // namespace mxhttp
