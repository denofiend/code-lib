/*
 *        Name: StringUtil.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <mxcore/Assert.h>
#include <mxcore/StringUtil.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

std::string& StringUtil::trim(std::string& str)
{
	trimLeft(str);
	return trimRight(str);
}

std::string& StringUtil::trimLeft(std::string& str)
{
	char ch;
	int pos = 0;

	if (str.empty())
	{
		return str;
	}

	while (pos < (int) str.length())
	{
		ch = str[pos];

		if ('\r' == ch || '\n' == ch || '\t' == ch || ' ' == ch)
		{
			pos++;
		}
		else
		{
			break;
		}
	}

	if (pos > 0)
	{
		str.erase(0, pos);
	}

	return str;
}

std::string& StringUtil::trimRight(std::string& str)
{
	int ch;
	int pos = 0;

	if (str.empty())
	{
		return str;
	}

	pos = str.length() - 1;

	while (pos >= 0)
	{
		ch = str[pos];

		if ('\r' == ch || '\n' == ch || '\t' == ch || ' ' == ch)
		{
			pos--;
		}
		else
		{
			break;
		}
	}

	pos++;

	if (pos < (int) str.length())
	{
		str.erase(pos);
	}

	return str;
}

bool StringUtil::startWith(const std::string& str, const std::string& with)
{
	MX_ASSERT(!with.empty());

	if (str.length() < with.length())
	{
		return false;
	}

	return 0 == ::strncmp(str.c_str(), with.c_str(), with.size());
}

bool StringUtil::endWith(const std::string& str, const std::string& with)
{
	MX_ASSERT(!with.empty());

	if (str.length() < with.length())
	{
		return false;
	}

	const char* s1 = str.c_str() + (str.length() - with.length());

	return 0 == ::strncmp(s1, with.c_str(), with.size());
}

int StringUtil::compare(const char* str1, const char* str2)
{
	MX_ASSERT(NULL != str1);
	MX_ASSERT(NULL != str2);

	return ::strcmp(str1, str2);
}

int StringUtil::compare(const std::string& str1, const std::string& str2)
{
	if (str1.empty())
	{
		return -1;
	}

	if (str2.empty())
	{
		return 1;
	}

	return compare(str1.c_str(), str2.c_str());
}

int StringUtil::compareN(const std::string& str1, const std::string& str2,
		int len)
{
	MX_ASSERT(len > 0);

	if (str1.empty())
	{
		return -1;
	}

	if (str2.empty())
	{
		return 1;
	}

	return compareN(str1.c_str(), str2.c_str(), len);
}

int StringUtil::compareN(const char* str1, const char* str2, int len)
{
	MX_ASSERT(len > 0);

	if (NULL == str1)
	{
		return -1;
	}

	if (NULL == str2)
	{
		return -1;
	}
	return ::strncmp(str1, str2, len);
}

int StringUtil::compareNoCase(const char* str1, const char* str2)
{
	if (NULL == str1)
	{
		return -1;
	}

	if (NULL == str2)
	{
		return 1;
	}

	return ::strcasecmp(str1, str2);
}

int StringUtil::compareNoCase(const std::string& str1, const std::string& str2)
{
	if (str1.empty() && str2.empty())
	{
		return 0;
	}

	if (str1.empty())
	{
		return -1;
	}

	if (str2.empty())
	{
		return 1;
	}

	return ::strcasecmp(str1.c_str(), str2.c_str());
}

int StringUtil::compareNoCaseN(const char* str1, const char* str2, int len)
{
	MX_ASSERT(len > 0);

	if (NULL == str1)
	{
		return -1;
	}

	if (NULL == str2)
	{
		return 1;
	}

	return ::strncasecmp(str1, str2, len);
}

int StringUtil::compareNoCaseN(const std::string& str1,
		const std::string& str2, int len)
{
	MX_ASSERT(len > 0);
	return compareNoCaseN(str1.c_str(), str2.c_str(), len);
}

bool StringUtil::equalNoCase(const std::string& str1, const std::string& str2)
{
	if (str1.empty() && str2.empty())
	{
		return true;
	}

	if (str1.length() != str2.length())
	{
		return false;
	}

	return 0 == ::strcasecmp(str1.c_str(), str2.c_str());
}

bool StringUtil::nextSeparator(const std::string& str, size_t& pos,
		const std::string& separator, std::string& subStr)
{
	if (pos == std::string::npos || pos >= str.length())
	{
		return false;
	}

	size_t pos1 = str.find(separator, pos);

	if (std::string::npos != pos1)
	{
		subStr = str.substr(pos, pos1 - pos);
		pos = pos1 + separator.length();
	}
	else
	{
		subStr = str.substr(pos);
		pos = std::string::npos;
	}
	return true;
}

int StringUtil::getLine(const void* buffer, int bufferLen, std::string& line)
{
	MX_ASSERT(NULL != buffer);

	line.clear();

	int lineLen = 0;
	const char* pos = (const char*) ::memchr(buffer, '\n', bufferLen);

	if (NULL != pos)
	{
		lineLen = pos - (const char*) buffer + 1;
		line.append((const char*) buffer, lineLen);
		trim(line);
	}

	return lineLen;
}

std::string& StringUtil::removeQuote(std::string& str)
{
	while ((str.length() >= 2) && ((str[0] == '"' && str[str.length() - 1]
			== '"')))
	{
		str.erase(str.length() - 1);
		str.erase(0, 1);
	}

	return str;
}

const char* StringUtil::memStr(const void* buf, int bufLen, const char* str,
		int strLen)
{
	MX_ASSERT(NULL != buf);
	MX_ASSERT(NULL != str);
	MX_ASSERT(strLen > 0);

	if (bufLen < strLen)
	{
		return NULL;
	}

	const char* p1 = (const char*) buf;
	const char* foundEnd = p1 + bufLen - strLen + 1;
	const char* p2 = str;

	for (; p1 < foundEnd;)
	{
		const char* pos = p1;

		while (*pos != *p2 && pos < foundEnd)
		{
			pos++;
		}

		if (pos >= foundEnd)
		{
			return NULL;
		}

		if (::memcmp(pos, str, strLen) == 0)
		{
			return pos;
		}

		p1 = pos + 1;
	}

	return NULL;
}

void StringUtil::toLower(std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		str[i] = ::tolower(str[i]);
	}
}

void StringUtil::toUpper(std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		str[i] = ::toupper(str[i]);
	}
}

} // namespace mxcore
