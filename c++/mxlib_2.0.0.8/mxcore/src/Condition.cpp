/*
 *        Name: Condition.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Condition.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

Condition::Condition(void)
{
	mxos::conditionInit(condition_);
}

Condition::~Condition(void)
{
	mxos::conditionDestroy(condition_);
}

void Condition::notify(void)
{
	mxos::conditionNotify(condition_);
}

void Condition::notifyAll(void)
{
	mxos::conditionNotifyAll(condition_);
}

void Condition::wait(MutexLock& mutex, long timeout)
{
	mxos::conditionWait(condition_, mutex.mutex_, timeout);
}

} // namespace mxcore
