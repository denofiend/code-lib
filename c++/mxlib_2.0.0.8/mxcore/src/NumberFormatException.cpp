/*
 *        Name: BadNumberException.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/NumberFormatException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

NumberFormatException::NumberFormatException(const std::string& message,
		int errorCode, const std::string& filename, int line) :
	RuntimeException(message, errorCode, filename, line)
{
}

NumberFormatException::NumberFormatException(const NumberFormatException& src) :
	RuntimeException(src)
{
}

NumberFormatException::~NumberFormatException(void)
{
}

NumberFormatException& NumberFormatException::operator =(
		const NumberFormatException& other)
{
	RuntimeException::operator=(other);
	return *this;
}

const std::string& NumberFormatException::name(void) const
{
	static const std::string sName("mxcore::NumberFormatException");
	return sName;
}

} // namespace mxcore
