/*
 *        Name: UrlCodec.cpp
 *
 *  Created on: 2011-2-15
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <math.h>
#include <mxcore/UrlCodec.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

// HEX Values array
const static char hexVals[16] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
		'F' };

// THIS IS A HELPER FUNCTION.
// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
static std::string decToHex(char num, int radix)
{
	int temp = 0;
	std::string csTmp;
	int num_char;
	num_char = (int) num;

	// ISO-8859-1
	// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A
	// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
	if (num_char < 0)
	{
		num_char = 256 + num_char;
	}

	while (num_char >= radix)
	{
		temp = num_char % radix;
		num_char = (int) ::floor(num_char / radix);
		csTmp = hexVals[temp];
	}

	csTmp += hexVals[num_char];

	if (csTmp.length() < 2)
	{
		csTmp += '0';
	}

	// Reverse the String
	std::string ret(csTmp);

	int n = 0;

	for (size_t i = csTmp.length(); i > 0; i--)
	{
		ret[i - 1] = csTmp[n++];
	}

	return ret;
}

// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
// TRUE = UNSAFE, FALSE = SAFE
static bool isUnsafe(char compareChar)
{
	// UNSAFE String
	static const std::string csUnsafeString("\"<>%\\^[]`+$,@:;/!#?=&");

	bool bcharfound = false;
	char tmpsafeChar;
	int m_strLen = 0;

	m_strLen = csUnsafeString.length();

	for (int ichar_pos = 0; ichar_pos < m_strLen; ichar_pos++)
	{
		tmpsafeChar = csUnsafeString[ichar_pos];

		if (tmpsafeChar == compareChar)
		{
			bcharfound = true;
			break;
		}
	}

	int char_ascii_value = 0;

	char_ascii_value = (int) compareChar;

	if (bcharfound == false && char_ascii_value > 32 && char_ascii_value < 123)
	{
		return false;
	}
	// found no unsafe chars, return false
	else
	{
		return true;
	}

	return true;
}

// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
static std::string convert(char val)
{
	std::string csRet("%");
	csRet += decToHex(val, 16);
	return csRet;
}

std::string UrlDecoder::decode(const std::string& str,
		const std::string& encoding)
{
	if (str.empty())
	{
		return str;
	}

	std::string tmp;

	const char* pstr = str.c_str();

	while ('\0' != *pstr)
	{
		if ('%' == *pstr)
		{
			pstr++;

			if (::isxdigit(pstr[0]) && ::isxdigit(pstr[1]))
			{
				char c1 = pstr[0];
				char c2 = pstr[1];
				pstr += 2;

				c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
				c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
				tmp += (unsigned char) (c1 * 16 + c2);
			}

			continue;
		}
		else if ('+' == *pstr)
		{
			tmp += ' ';
		}
		else
		{
			tmp += *pstr;
		}

		pstr++;
	}

	return tmp;
}

std::string UrlEncoder::encode(const std::string& str,
		const std::string& encoding)
{
	int ichar_pos;
	std::string csEncode(str);
	std::string csEncoded;
	int m_length;

	m_length = csEncode.length();

	for (ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
	{
		char ch = csEncode[ichar_pos];

		if (ch == ' ')
		{
			ch = '+';
			csEncoded += ch;
			continue;
		}

		if (!isUnsafe(ch))
		{
			// Safe Character
			csEncoded += ch;
		}
		else
		{
			// get Hex Value of the Character
			csEncoded += convert(ch);
		}
	}

	return csEncoded;
}

} // namespace mxcore
