/*
 *        Name: IOService.cpp
 *
 *  Created on: 2011-2-24
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxasio/IOService.h>
#include <mxasio/libevent/IOServiceLibEvent.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

IOService* IOService::createInstance(void) throw (mxcore::IOException&)
{
	IOServiceLibEvent* ret = new IOServiceLibEvent;

	try
	{
		ret->open();
		return ret;
	} catch (mxcore::IOException& e)
	{
		delete ret;
		throw e;
	}
}

} // namespace mxasio
