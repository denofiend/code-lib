/*
 * Base64.cpp
 *
 *  Created on: 2011-1-10
 *      Author: zjg
 */

#include <string>
#include <string.h>
#include <mxcore/Assert.h>
#include <mxcore/Base64.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

static const char* B64_TABLE =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned int B64_TABLE_LEN = ::strlen(B64_TABLE);
static const unsigned int B64_LINE_LEN = 76;

static const char B64_DEC_TABLE[] =
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
		59, 60, 61, -1, -1, -1, 0, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
		-1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 };

static inline unsigned int encodeImpl(const char* src, unsigned int len,
		std::string& encoded)
{
	MX_ASSERT(len > 0);
	char tmp[4] =
	{ '=', '=', '=', '=' };

	int n = len >= 3 ? 3 : len;

	tmp[0] = B64_TABLE[((int) src[0] & 0xFC) >> 2];

	switch (n)
	{
	case 1:
		tmp[1] = B64_TABLE[((int) src[0] & 0x3) << 4];
		break;
	case 2:
		tmp[1] = B64_TABLE[((int) src[0] & 0x3) << 4 | ((int) src[1] & 0xF0)
				>> 4];
		tmp[2] = B64_TABLE[((int) src[1] & 0x0F) << 2];
		break;
	case 3:
		tmp[1] = B64_TABLE[((int) src[0] & 0x3) << 4 | ((int) src[1] & 0xF0)
				>> 4];
		tmp[2] = B64_TABLE[((int) src[1] & 0xF) << 2 | ((int) src[2] & 0xC0)
				>> 6];
		tmp[3] = B64_TABLE[(int) src[2] & 0x3F];
		break;
	default:
		break;
	};

	encoded.append(tmp, 4);
	return n;
}

std::string Base64::encode(const char* src, unsigned int len)
{
	/*
	 * 以下是操作二进制数时用到的
	 * 11111100 0xFC
	 * 11000000 0x3
	 * 11110000 0xF0
	 * 00001111 0xF
	 * 11000000 0xC0
	 * 00111111 0x3F
	 */

	std::string encoded;

	MX_ASSERT(NULL != src);

	while (len > 0)
	{
		unsigned int n = encodeImpl(src, len, encoded);
		src += n;
		len -= n;
	}
	return encoded;
}

std::string Base64::encode(const std::string& src)
{
	return encode(src.c_str(), src.size());
}

static char DEC_BYTE(const char* bytes, int index)
{
	return B64_DEC_TABLE[(int) bytes[index]];
}

static inline unsigned int decodeImpl(const char* bytes, unsigned int len,
		std::string& decoded)
{
	char tmp[4] =
	{ '=', '=', '=', '=' };

	MX_ASSERT(len > 0);

	int n = len >= 4 ? 4 : len;

	::memcpy(tmp, bytes, n);

	if ('=' == tmp[2] && '=' == tmp[3])
	{
		decoded += (char) (int) (DEC_BYTE(bytes, 0) << 2 | DEC_BYTE(bytes, 1)
				<< 2 >> 6);
	}
	else if ('=' == tmp[3])
	{
		decoded += (char) (int) (DEC_BYTE(bytes, 0) << 2 | DEC_BYTE(bytes, 1)
				<< 2 >> 6);
		decoded += (char) (int) (DEC_BYTE(bytes, 1) << 4 | DEC_BYTE(bytes, 2)
				<< 2 >> 4);
	}
	else
	{
		decoded += (char) (int) (DEC_BYTE(bytes, 0) << 2 | DEC_BYTE(bytes, 1)
				<< 2 >> 6);
		decoded += (char) (int) (DEC_BYTE(bytes, 1) << 4 | DEC_BYTE(bytes, 2)
				<< 2 >> 4);
		decoded += (char) (int) (DEC_BYTE(bytes, 2) << 6 | DEC_BYTE(bytes, 3)
				<< 2 >> 2);
	}

	return n;
}

std::string Base64::decode(const char* src, unsigned int len)
{
	std::string decoded;

	while (len > 0)
	{
		unsigned int decodedLen = decodeImpl(src, len, decoded);
		src += decodedLen;
		len -= decodedLen;
	}
	return decoded;
}

std::string Base64::decode(const std::string& src)
{
	return decode(src.c_str(), src.size());
}

} // namespace mxcore
