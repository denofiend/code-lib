/*
 *        Name: NotImplException.cpp
 *
 *  Created on: 2011-2-1
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/NotImplException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

NotImplException::NotImplException(const std::string& message,
		int errorCode, const std::string& filename, int line) :
	RuntimeException(message, errorCode, filename, line)
{
}

NotImplException::NotImplException(const NotImplException& src) :
	RuntimeException(src)
{
}

NotImplException::~NotImplException(void)
{
}

NotImplException& NotImplException::operator =(
		const NotImplException& other)
{
	RuntimeException::operator=(other);
	return *this;
}

const std::string& NotImplException::name(void) const
{
	static const std::string sName("mxcore::NotImplException");
	return sName;
}

} // namespace mxcore
