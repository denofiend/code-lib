/*
 * XmlException.cpp
 *
 *  Created on: 2011-3-7
 *      Author: zhaojiangang
 */

#include <mxcore/XmlException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

XmlException::XmlException(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	Exception(message, errorCode, filename, line)
{
}

XmlException::XmlException(const XmlException& src) :
	Exception(src)
{
}

XmlException::~XmlException(void)
{
}

XmlException& XmlException::operator =(const XmlException& other)
{
	Exception::operator=(other);
	return *this;
}

const std::string& XmlException::name(void) const
{
	static const std::string sName("mxcore::XmlException");
	return sName;
}

} // namespace mxcore
