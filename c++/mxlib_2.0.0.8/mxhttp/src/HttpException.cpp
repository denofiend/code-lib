/*
 *        Name: HttpException.cpp
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxhttp/HttpException.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpException::HttpException(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	Exception(message, errorCode, filename, line), httpStatus_(
			HttpConstants::STATUS_BAD_REQUEST)
{
}

HttpException::HttpException(const std::string& message, int errorCode,
		const std::string& filename, int line,
		const std::pair<int, std::string>& httpStatus) :
	Exception(message, errorCode, filename, line), httpStatus_(httpStatus)
{
}

HttpException::HttpException(const HttpException& src) :
	Exception(src), httpStatus_(src.httpStatus_)
{
}

HttpException::~HttpException(void)
{
}

HttpException& HttpException::operator =(const HttpException& other)
{
	if (this != &other)
	{
		Exception::operator=(other);
		httpStatus_ = other.httpStatus_;
	}
	return *this;
}

const std::string& HttpException::name(void) const
{
	static const std::string sName("mxhttp::HttpException");
	return sName;
}

} // namespace mxhttp
