/*
 *        Name: UnknownHostException.cpp
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/UnknownHostException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

UnknownHostException::UnknownHostException(const std::string& message,
		int errorCode, const std::string& filename, int line) :
	IOException(message, errorCode, filename, line)
{
}

UnknownHostException::UnknownHostException(const UnknownHostException& src) :
	IOException(src)
{
}

UnknownHostException::~UnknownHostException(void)
{
}

UnknownHostException& UnknownHostException::operator =(
		const UnknownHostException& other)
{
	IOException::operator=(other);
	return *this;
}

const std::string& UnknownHostException::name(void) const
{
	static const std::string sName("mxcore::UnknownHostException");
	return sName;
}

} // namespace mxcore
