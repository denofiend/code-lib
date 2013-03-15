/*
 *        Name: RuntimeException.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_RUNTIMEEXCEPTION_H__
#define __MXCORE_RUNTIMEEXCEPTION_H__

#include <mxcore/Exception.h>

namespace mxcore
{

class RuntimeException: public Exception
{
public:
	RuntimeException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	RuntimeException(const RuntimeException& src);
	virtual ~RuntimeException(void);
	RuntimeException& operator =(const RuntimeException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_RUNTIMEEXCEPTION_H__ */
