/*
 *        Name: Condition.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_CONDITION_H__
#define __MXCORE_CONDITION_H__

#include <mxcore/ThreadOps.h>
#include <mxcore/MutexLock.h>

namespace mxcore
{

class Condition
{
public:
	Condition(void);
	~Condition(void);

	void notify(void);
	void notifyAll(void);
	void wait(MutexLock& mutex, long timeout = -1);
private:
	mxos::ConditionT condition_;
};

} // namespace mxcore

#endif /* __MXCORE_CONDITION_H__ */
