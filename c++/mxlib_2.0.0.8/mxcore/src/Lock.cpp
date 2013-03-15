/*
 *        Name: Lock.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Lock.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

Lock::Lock(void)
{
}

Lock::~Lock(void)
{
}

Lock::ScopeLock::ScopeLock(Lock& lock) :
	lock_(lock)
{
	lock_.lock();
}

Lock::ScopeLock::~ScopeLock(void)
{
	lock_.unlock();
}

FakeLock::FakeLock(void)
{
}

FakeLock::~FakeLock(void)
{
}

void FakeLock::lock(void)
{
}

bool FakeLock::tryLock(void)
{
	return true;
}

void FakeLock::unlock(void)
{
}

} // namespace mxcore
