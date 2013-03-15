/*
 * Base64.h
 *
 *  Created on: 2011-1-10
 *      Author: zjg
 */

#ifndef __MXCORE_BASE64_H__
#define __MXCORE_BASE64_H__

#include <string>

namespace mxcore
{

class Base64
{
private:
	Base64(void);
public:
	static std::string encode(const std::string& src);
	static std::string encode(const char* src, unsigned int len);

	static std::string decode(const std::string& src);
	static std::string decode(const char* src, unsigned int len);
};

} // namespace mxcore

#endif /* __MXCORE_BASE64_H__ */
