/*
 *        Name: HttpBody.h
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPBODY_H__
#define __MXHTTP_HTTPBODY_H__

#include <mxhttp/HttpPairList.h>
#include <mxcore/IOException.h>

namespace mxhttp
{

class HttpBody
{
protected:
	HttpBody(void);
public:
	virtual ~HttpBody(void);

	void close(void);
	bool isClosed(void) const;

	virtual bool isSimpleBody(void) const;
	virtual bool isMultipartBody(void) const;
protected:
	virtual void closeImpl(void) = 0;
private:
	bool closed_;
};

inline void HttpBody::close(void)
{
	if (!closed_)
	{
		closed_ = true;
		closeImpl();
	}
}

inline bool HttpBody::isClosed(void) const
{
	return closed_;
}

} // namespace mxhttp

#endif /* __MXHTTP_HTTPBODY_H__ */
