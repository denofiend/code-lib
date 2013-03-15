/*
 *        Name: Exception.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Exception.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

Exception::Exception(const std::string& message, int errorCode,
		const std::string& filename, int line) :
	message_(message), errorCode_(errorCode), filename_(filename), line_(line)
{
}

Exception::Exception(const Exception& src) :
	message_(src.message_), errorCode_(src.errorCode_),
			filename_(src.filename_), line_(src.line_)
{
}

Exception::~Exception(void)
{
}

Exception& Exception::operator =(const Exception& other)
{
	if (this != &other)
	{
		message_ = other.message_;
		errorCode_ = other.errorCode_;
		filename_ = other.filename_;
		line_ = other.line_;
	}
	return *this;
}

const std::string& Exception::name(void) const
{
	static const std::string sName("mxcore::Exception");
	return sName;
}

} // namespace mxcore
