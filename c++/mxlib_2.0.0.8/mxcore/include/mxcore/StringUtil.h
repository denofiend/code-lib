/*
 *        Name: StringUtil.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_STRINGUTIL_H__
#define __MXCORE_STRINGUTIL_H__

#include <string>

namespace mxcore
{

class StringUtil
{
private:
	StringUtil(void);
public:
	static std::string& trim(std::string& str);
	static std::string& trimLeft(std::string& str);
	static std::string& trimRight(std::string& str);

	static bool startWith(const std::string& str, const std::string& with);
	static bool endWith(const std::string& str, const std::string& with);

	static int compare(const char* str1, const char* str2);
	static int compare(const std::string& str1, const std::string& str2);

	static int compareN(const char* str1, const char* str2, int len);
	static int compareN(const std::string& str1, const std::string& str2,
			int len);

	static int compareNoCase(const char* str1, const char* str2);
	static int compareNoCase(const std::string& str1, const std::string& str2);

	static int compareNoCaseN(const char* str1, const char* str2, int len);
	static int compareNoCaseN(const std::string& str1, const std::string& str2,
			int len);

	static bool equalNoCase(const std::string& str1, const std::string& str2);

	static bool nextSeparator(const std::string& str, size_t& pos,
			const std::string& separator, std::string& subStr);

	static int getLine(const void* buffer, int bufferLen, std::string& line);
	static std::string& removeQuote(std::string& str);

	static const char* memStr(const void* buf, int bufLen, const char* str,
			int strLen);

	static void toLower(std::string& str);
	static void toUpper(std::string& str);
};

} // namespace mxcore

#endif /* __MXCORE_STRINGUTIL_H__ */
