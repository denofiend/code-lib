/*
 *        Name: Thread.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Thread.h>
#include <mxcore/Assert.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

Thread::Thread(Runnable* target, bool managerTarget) :
	isStart_(false), target_(target), managerTarget_(managerTarget)
{
}

Thread::~Thread(void)
{
	if (NULL != target_ && managerTarget_)
	{
		delete target_;
	}
}

void Thread::start(void) throw (SystemException&)
{
	MX_ASSERT(!isStart_);
	isStart_ = true;

	if (0 != mxos::threadStart(thread_, Thread::threadProc, this))
	{
		// TODO throw
	}
}

void Thread::join(void) throw (SystemException&)
{
	MX_ASSERT(isStart_);

	mxos::threadJoin(thread_);
}

long Thread::self(void) const
{
	return mxos::threadSelf(thread_);
}

long Thread::current(void)
{
	return mxos::threadCurrent();
}

void Thread::sleep(long timeout)
{
	return mxos::threadSleep(timeout);
}

void Thread::run(void)
{
	if (NULL != target_)
	{
		target_->run();
	}
}

void Thread::notifyStop(void)
{
}

void* Thread::threadProc(void* param)
{
	Runnable* runner = (Runnable*) param;
	runner->run();
	return 0;
}

} // namespace mxcore
