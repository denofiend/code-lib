/*
 * CodeConvertException.cpp
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#include <mxcore/CodeConvertException.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

CodeConvertException::CodeConvertException(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	Exception(message, errorCode, filename, line)
{
}

CodeConvertException::CodeConvertException(const CodeConvertException& src) :
	Exception(src)
{
}

CodeConvertException::~CodeConvertException(void)
{
}

CodeConvertException& CodeConvertException::operator =(const CodeConvertException& other)
{
	Exception::operator=(other);
	return *this;
}

const std::string& CodeConvertException::name(void) const
{
	static const std::string sName("mxcore::CodeConvertException");
	return sName;
}

} // namespace mxcore
