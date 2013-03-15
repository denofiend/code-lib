/*
 *        Name: Thread.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_THREAD_H__
#define __MXCORE_THREAD_H__

#include <mxcore/ThreadOps.h>
#include <mxcore/Runnable.h>
#include <mxcore/SystemException.h>

namespace mxcore
{

class Thread: public Runnable
{
public:
	Thread(Runnable* target, bool managerTarget = false);
	virtual ~Thread(void);

	void start(void) throw (SystemException&);
	void join(void) throw (SystemException&);

	long self(void) const;

	static long current(void);
	static void sleep(long timeout);

	virtual void run(void);
	virtual void notifyStop(void);
private:
	static void* threadProc(void* param);
private:
	bool isStart_;
	Runnable* target_;
	bool managerTarget_;
	mxos::ThreadT thread_;
};

} // namespace mxcore

#endif /* __MXCORE_THREAD_H__ */
