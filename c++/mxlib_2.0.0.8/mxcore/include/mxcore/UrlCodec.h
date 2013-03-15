/*
 *        Name: UrlCodec.h
 *
 *  Created on: 2011-2-15
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_URLCODEC_H__
#define __MXCORE_URLCODEC_H__

#include <string>

namespace mxcore
{

class UrlEncoder
{
private:
	UrlEncoder(void);
public:
	static std::string encode(const std::string& str,
			const std::string& encoding = "UTF-8");
};

class UrlDecoder
{
private:
	UrlDecoder(void);
public:
	static std::string decode(const std::string& str,
			const std::string& encoding = "UTF-8");
};

} // namespace mxcore

#endif /* __MXCORE_URLCODEC_H__ */
