/*
 *        Name: NotImplException.h
 *
 *  Created on: 2011-2-1
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_NOTIMPLEXCEPTION_H__
#define __MXCORE_NOTIMPLEXCEPTION_H__

#include <mxcore/RuntimeException.h>

namespace mxcore
{

class NotImplException: public RuntimeException
{
public:
	NotImplException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	NotImplException(const NotImplException& src);
	virtual ~NotImplException(void);
	NotImplException& operator =(const NotImplException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_NOTIMPLEXCEPTION_H__ */
