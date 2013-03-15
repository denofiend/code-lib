/*
 * TaskQueue.h
 *
 *  Created on: 2011-11-7
 *      Author: DenoFiend
 */

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <list>
#include <tr1/memory>
#include <mxcore/Thread.h>
#include <mxcore/Condition.h>
#include <mxcore/MutexLock.h>

typedef std::tr1::shared_ptr<mxcore::Runnable> TaskPtr;

namespace mx_mul
{

class TaskQueue
{
public:
	TaskQueue(void)
	{
	}

	virtual ~TaskQueue(void)
	{
	}

	TaskPtr pop(void)
	{
		mxcore::Lock::ScopeLock sl(lock_);

		while (tasks_.empty())
		{
			condition_.wait(lock_, -1);
		}

		TaskPtr task = tasks_.front();
		tasks_.pop_front();
		return task;
	}

	void push(TaskPtr task)
	{
		mxcore::Lock::ScopeLock sl(lock_);
		tasks_.push_back(task);
		condition_.notify();
	}
private:
	std::list<TaskPtr> tasks_;
	mxcore::AutoMutexLock lock_;
	mxcore::Condition condition_;
};

} /* namespace skyfile */
#endif /* TASKQUEUE_H_ */
