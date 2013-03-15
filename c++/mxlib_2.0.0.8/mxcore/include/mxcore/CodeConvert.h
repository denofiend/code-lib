/*
 * CodeConvert.h
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#ifndef __MXCORE_CODECONVERT_H__
#define __MXCORE_CODECONVERT_H__

#include <string>
#include <iconv.h>
#include <mxcore/CodeConvertException.h>

namespace mxcore
{

class CodeConvert
{
public:
	CodeConvert(void);
	~CodeConvert(void);

	void close(void);
	bool isOpen(void) const;

	void open(const std::string& from, const std::string& to)
			throw (mxcore::CodeConvertException&);

	size_t
			convert(const char* inbuf, size_t inlen, char* outbuf,
					size_t outlen) throw (mxcore::CodeConvertException&);
private:
	static const iconv_t kInvalidCd;
private:
	std::string from_;
	std::string to_;

	iconv_t cd_;
};

inline bool CodeConvert::isOpen(void) const
{
	return kInvalidCd != cd_;
}

} // namespace mxcore

#endif /* __MXCORE_CODECONVERT_H__ */
