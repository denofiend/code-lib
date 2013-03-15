/*
 *        Name: NumberFormatException
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_NUMBERFORMATEXCEPTION_H__
#define __MXCORE_NUMBERFORMATEXCEPTION_H__

#include <mxcore/RuntimeException.h>

namespace mxcore
{

class NumberFormatException: public RuntimeException
{
public:
	NumberFormatException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	NumberFormatException(const NumberFormatException& src);
	virtual ~NumberFormatException(void);
	NumberFormatException& operator =(const NumberFormatException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_NUMBERFORMATEXCEPTION_H__ */
