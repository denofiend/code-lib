/*
 * TaskThread.h
 *
 *  Created on: 2011-11-7
 *      Author: DenoFiend
 */

#ifndef TASKTHREAD_H_
#define TASKTHREAD_H_

#include "mul_task/Shared.h"
#include <mxcore/Thread.h>

namespace mx_mul
{

class TaskThread: public mxcore::Thread
{
public:
	TaskThread(TaskQueuePtr queue) :
			mxcore::Thread(NULL, false), queue_(queue)
	{
	}

	virtual ~TaskThread(void)
	{
	}

	virtual void run(void)
	{
		for (;;)
		{
			TaskPtr task = queue_->pop();

			if (task.get())
			{
				task->run();
			}
			else
			{
				break;
			}
		}
	}
private:
	TaskQueuePtr queue_;
};

typedef std::tr1::shared_ptr<mx_mul::TaskThread> TaskThreadPtr;

} /* namespace skyfile */
#endif /* TASKTHREAD_H_ */
