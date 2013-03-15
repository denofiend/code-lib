/*
 *        Name: UnknownHostException.h
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_UNKNOWNHOSTEXCEPTION_H__
#define __MXCORE_UNKNOWNHOSTEXCEPTION_H__

#include <mxcore/IOException.h>

namespace mxcore
{

class UnknownHostException: public IOException
{
public:
	UnknownHostException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	UnknownHostException(const UnknownHostException& src);
	virtual ~UnknownHostException(void);
	UnknownHostException& operator =(const UnknownHostException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_UNKNOWNHOSTEXCEPTION_H__ */
