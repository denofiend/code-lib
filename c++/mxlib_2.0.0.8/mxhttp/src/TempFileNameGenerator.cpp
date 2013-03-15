/*
 *        Name: TempFileNameGenerator.cpp
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/SystemOps.h>
#include <mxhttp/TempFileNameGenerator.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

TempFileNameGeneratorSimple::TempFileNameGeneratorSimple(
		const std::string& rootPath) :
	fileNo_(0), rootPath_(rootPath)
{
	if (rootPath_.empty())
	{
		rootPath_ = "./";
	}
	else if (rootPath_[rootPath_.length() - 1] != '/')
	{
		rootPath_ += "/";
	}
}

TempFileNameGeneratorSimple::~TempFileNameGeneratorSimple(void)
{
}

std::string TempFileNameGeneratorSimple::getNextFileName(void)
{
	char tmp[64];
	int32_t count = fileNo_.increment();
	::snprintf(tmp, 64, "%u.%u.%u.tmp", (unsigned int) mxos::getPid(),
			(unsigned int) time(NULL), (unsigned int) count);
	std::string s(rootPath_);
	s += tmp;
	return s;
}

} // namespace mxhttp
