/*
 *        Name: HttpBody.cpp
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxhttp/HttpBody.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpBody::HttpBody(void) :
	closed_(false)
{
}

HttpBody::~HttpBody(void)
{
}

bool HttpBody::isSimpleBody(void) const
{
	return false;
}

bool HttpBody::isMultipartBody(void) const
{
	return false;
}
//
//HttpSimpleBody::HttpSimpleBody(void)
//{
//}
//
//HttpSimpleBody::~HttpSimpleBody(void)
//{
//}
//
//bool HttpSimpleBody::isSimpleBody(void) const
//{
//	return true;
//}
//
//HttpSimpleBodyInMem::HttpSimpleBodyInMem(const std::string& content) :
//	content_(content), readPos_(0), writePos_((int32_t) content.size()),
//			state_(HttpSimpleBody::eWriting), closed_(false)
//{
//}
//
//HttpSimpleBodyInMem::~HttpSimpleBodyInMem(void)
//{
//	close();
//}
//
//HttpSimpleBodyInMem* HttpSimpleBodyInMem::createInstance(
//		const std::string& content)
//{
//}
//
//void HttpSimpleBodyInMem::flip(void) throw (mxcore::IOException&)
//{
//}
//int HttpSimpleBodyInMem::read(void* buffer, int len)
//		throw (mxcore::IOException&)
//{
//}
//int HttpSimpleBodyInMem::write(const void* buffer, int len)
//		throw (mxcore::IOException&)
//{
//}

} // namespace mxhttp
