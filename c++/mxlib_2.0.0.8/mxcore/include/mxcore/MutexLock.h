/*
 *        Name: MutexLock.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_MUTEXLOCK_H__
#define __MXCORE_MUTEXLOCK_H__

#include <mxcore/Lock.h>
#include <mxcore/ThreadOps.h>

namespace mxcore
{

class MutexLock: public Lock
{
	friend class Condition;
private:
	MutexLock(const MutexLock&);
	MutexLock& operator =(const MutexLock&);
public:
	MutexLock(void);
	virtual ~MutexLock(void);

	void init(void);
	void destroy(void);

	virtual void lock(void);
	virtual bool tryLock(void);
	virtual void unlock(void);
private:
	mxos::MutexT mutex_;
};

class AutoMutexLock: public MutexLock
{
private:
	AutoMutexLock(const AutoMutexLock&);
	AutoMutexLock& operator = (const AutoMutexLock&);
public:
	AutoMutexLock(void);
	virtual ~AutoMutexLock(void);
private:
	void init(void);
	void destroy(void);
};

} // namespace mxcore

#endif /* __MXCORE_MUTEXLOCK_H__ */
