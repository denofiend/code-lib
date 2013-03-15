/*
 *        Name: HttpParseException.h
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPPARSEEXCEPTION_H__
#define __MXHTTP_HTTPPARSEEXCEPTION_H__

#include <mxhttp/HttpException.h>

namespace mxhttp
{

class HttpParseException: public HttpException
{
public:
	HttpParseException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	HttpParseException(const std::string& message, int errorCode,
			const std::string& filename, int line, const std::pair<int,
					std::string>& httpStatus);
	HttpParseException(const HttpParseException& src);

	virtual ~HttpParseException(void);
	HttpParseException& operator =(const HttpParseException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPPARSEEXCEPTION_H__ */
