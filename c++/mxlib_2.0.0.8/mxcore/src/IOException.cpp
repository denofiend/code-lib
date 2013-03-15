/*
 *        Name: IOException.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/IOException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

IOException::IOException(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	Exception(message, errorCode, filename, line)
{
}

IOException::IOException(const IOException& src) :
	Exception(src)
{
}

IOException::~IOException(void)
{
}

IOException& IOException::operator =(const IOException& other)
{
	Exception::operator=(other);
	return *this;
}

const std::string& IOException::name(void) const
{
	static const std::string sName("mxcore::IOException");
	return sName;
}

} // namespace mxcore
