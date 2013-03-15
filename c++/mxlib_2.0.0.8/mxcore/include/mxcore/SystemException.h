/*
 *        Name: SystemException.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_SYSTEMEXCEPTION_H__
#define __MXCORE_SYSTEMEXCEPTION_H__

#include <mxcore/RuntimeException.h>

namespace mxcore
{

class SystemException: public RuntimeException
{
public:
	SystemException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	SystemException(const SystemException& src);
	virtual ~SystemException(void);
	SystemException& operator =(const SystemException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_SYSTEMEXCEPTION_H__ */
