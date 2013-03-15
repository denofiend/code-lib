/*
 * CodeConvert.cpp
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#include "../../config.h"
#include <mxcore/SystemOps.h>

#include <mxcore/Assert.h>
#include <mxcore/StringUtil.h>
#include <mxcore/CodeConvert.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

const iconv_t CodeConvert::kInvalidCd = (void*)-1;

CodeConvert::CodeConvert(void) :
	cd_(NULL)
{
}

CodeConvert::~CodeConvert(void)
{
	close();
}

void CodeConvert::close(void)
{
	if (isOpen())
	{
		::iconv_close(cd_);
		cd_ = kInvalidCd;

		from_.clear();
		to_.clear();
	}
}

void CodeConvert::open(const std::string& from, const std::string& to)
		throw (mxcore::CodeConvertException&)
{
	MX_ASSERT(!from.empty() && !to.empty());

	if (isOpen() && mxcore::StringUtil::equalNoCase(from, from_)
			&& mxcore::StringUtil::equalNoCase(to, to_))
	{
		return;
	}

	close();

	cd_ = ::iconv_open(to.c_str(), from.c_str());

	if (kInvalidCd == cd_)
	{
		THROW2(CodeConvertException
				, std::string("Unsupport from: [") + from + "] to [" + to + "]");
	}

	from_ = from;
	to_ = to;
}

size_t CodeConvert::convert(const char* inbuf, size_t inlen, char* outbuf,
		size_t outlen) throw (mxcore::CodeConvertException&)
{
	MX_ASSERT(isOpen());

#if !defined(ICONV_CONST)
	char** p = (char**) &inbuf;
#else
	const char** p = (const char**)&inbuf;
#endif
	size_t ret = ::iconv(cd_, p, &inlen, &outbuf, &outlen);

	if (ret < 0)
	{
		THROW3(CodeConvertException
				, "Failed to convert", mxos::getLastError());
	}

	return ret;
}

} // namespace mxcore
