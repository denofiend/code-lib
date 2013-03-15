/*
 *        Name: MutexLock.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/MutexLock.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

MutexLock::MutexLock(void)
{
}

MutexLock::~MutexLock(void)
{
}

void MutexLock::init(void)
{
	mxos::mutexInit(mutex_);
}

void MutexLock::destroy(void)
{
	mxos::mutexDestroy(mutex_);
}

void MutexLock::lock(void)
{
	mxos::mutexLock(mutex_);
}

bool MutexLock::tryLock(void)
{
	return mxos::mutexTryLock(mutex_);
}

void MutexLock::unlock(void)
{
	mxos::mutexUnlock(mutex_);
}

AutoMutexLock::AutoMutexLock(void)
{
	MutexLock::init();
}

AutoMutexLock::~AutoMutexLock(void)
{
	MutexLock::destroy();
}

} // namespace mxcore
