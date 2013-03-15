/*
 *        Name: RuntimeException.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/RuntimeException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

RuntimeException::RuntimeException(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	Exception(message, errorCode, filename, line)
{
}

RuntimeException::RuntimeException(const RuntimeException& src) :
	Exception(src)
{
}

RuntimeException::~RuntimeException(void)
{
}

RuntimeException& RuntimeException::operator =(const RuntimeException& other)
{
	Exception::operator=(other);
	return *this;
}

const std::string& RuntimeException::name(void) const
{
	static const std::string sName("mxcore::RuntimeException");
	return sName;
}

} // namespace mxcore
