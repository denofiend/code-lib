/*
 *        Name: IOException.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_IOEXCEPTION_H__
#define __MXCORE_IOEXCEPTION_H__

#include <mxcore/Exception.h>

namespace mxcore
{

class IOException: public Exception
{
public:
	IOException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	IOException(const IOException& src);
	virtual ~IOException(void);
	IOException& operator =(const IOException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_IOEXCEPTION_H__ */
