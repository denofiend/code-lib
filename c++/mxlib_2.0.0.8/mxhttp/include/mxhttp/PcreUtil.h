/*
 * PcreUtil.h
 *
 *  Created on: 2011-3-11
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_PCREUTIL_H__
#define __MXHTTP_PCREUTIL_H__

#include <pcre.h>
#include <string>
#include <mxcore/Assert.h>

namespace mxhttp
{

class PcreUtil
{
	PcreUtil(void);
public:
	static pcre* open(const char* pattern);
	static pcre* open(const std::string& pattern);
	static int match(pcre* re, const std::string& str);
	static int match(pcre* re, const char* str, size_t len);
	static void close(pcre* re);
};

inline pcre* PcreUtil::open(const char* pattern)
{
	pcre* re;
	const char* error;
	int erroffset;

	re = pcre_compile(pattern, /* the pattern */
	0, /* default options */
	&error, /* for error message */
	&erroffset, /* for error offset */
	NULL); /* use default character tables */

	return re;
}

inline pcre* PcreUtil::open(const std::string& pattern)
{
	return open(pattern.c_str());
}

inline int PcreUtil::match(pcre* re, const std::string& str)
{
	return match(re, str.c_str(), str.length());
}

inline int PcreUtil::match(pcre* re, const char* str, size_t len)
{
	MX_ASSERT(NULL != re);
	MX_ASSERT(NULL != str);
	MX_ASSERT(len > 0);

	return pcre_exec(re, /* the compiled pattern */
	NULL, /* no extra data - we didn't study the pattern */
	str, /* the src string */
	len, /* the length of the src */
	0, /* start at offset 0 in the src */
	0, /* default options */
	NULL, 0);
}

inline void PcreUtil::close(pcre* re)
{
	if (NULL != re)
	{
		::free(re);
	}
}

} // namespace mxhttp

#endif /* __MXHTTP_PCREUTIL_H__ */
