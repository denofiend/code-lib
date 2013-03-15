/*
 * XmlException.h
 *
 *  Created on: 2011-3-7
 *      Author: zhaojiangang
 */

#ifndef __MXCORE_XMLEXCEPTION_H__
#define __MXCORE_XMLEXCEPTION_H__

#include <mxcore/Exception.h>

namespace mxcore
{

class XmlException: public Exception
{
public:
	XmlException(const std::string& message, int errorCode,
			const std::string& filename, int line);
	XmlException(const XmlException& src);
	virtual ~XmlException(void);
	XmlException& operator =(const XmlException& other);

	virtual const std::string& name(void) const;
};

} // namespace mxcore

#endif /* __MXCORE_XMLEXCEPTION_H__ */
