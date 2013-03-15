/*
 * CodeConvertException.h
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#ifndef __MXCORE_CODECONVERTEXCEPTION_H__
#define __MXCORE_CODECONVERTEXCEPTION_H__

#include <mxcore/Exception.h>

namespace mxcore
{

class CodeConvertException: public Exception
{
public:
	CodeConvertException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	CodeConvertException(const CodeConvertException& src);
	virtual ~CodeConvertException(void);
	CodeConvertException& operator =(const CodeConvertException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_CODECONVERTEXCEPTION_H__ */
