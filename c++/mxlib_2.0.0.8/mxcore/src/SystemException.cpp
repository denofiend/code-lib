/*
 *        Name: SystemException.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/SystemException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

SystemException::SystemException(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	RuntimeException(message, errorCode, filename, line)
{
}

SystemException::SystemException(const SystemException& src) :
	RuntimeException(src)
{
}

SystemException::~SystemException(void)
{
}

SystemException& SystemException::operator =(const SystemException& other)
{
	RuntimeException::operator=(other);
	return *this;
}

const std::string& SystemException::name(void) const
{
	static const std::string sName("mxcore::SystemException");
	return sName;
}

} // namespace mxcore
