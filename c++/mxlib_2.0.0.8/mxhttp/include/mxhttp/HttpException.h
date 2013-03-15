/*
 *        Name: HttpException.h
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPEXCEPTION_H__
#define __MXHTTP_HTTPEXCEPTION_H__

#include <mxcore/Exception.h>
#include <mxhttp/HttpConstants.h>

#define THROW_HTTP_EXCEPTION2(e, msg) \
		throw e((msg), 0, __FILE__, __LINE__);

#define THROW_HTTP_EXCEPTION3(e, msg, s) \
		throw e((msg), 0, __FILE__, __LINE__, (s));

#define THROW_HTTP_EXCEPTION4(e, msg, ec, s) \
		throw e((msg), (ec), __FILE__, __LINE__, (s));

namespace mxhttp
{

class HttpException: public mxcore::Exception
{
public:
	HttpException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	HttpException(const std::string& message, int errorCode,
			const std::string& filename, int line, const std::pair<int,
					std::string>& httpStatus);
	HttpException(const HttpException& src);

	virtual ~HttpException(void);
	HttpException& operator =(const HttpException& other);

	virtual const std::string& name(void) const;

	const std::pair<int, std::string>& getHttpStatus(void) const;
	void setHttpStatus(const std::pair<int, std::string>& httpStatus);
private:
	std::pair<int, std::string> httpStatus_;
};

inline const std::pair<int, std::string>& HttpException::getHttpStatus(void) const
{
	return httpStatus_;
}

inline void HttpException::setHttpStatus(
		const std::pair<int, std::string>& httpStatus)
{
	httpStatus_ = httpStatus;
}

} // namespace mxhttp

#endif /* __MXHTTP_HTTPEXCEPTION_H__ */
