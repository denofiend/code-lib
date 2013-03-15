/*
 *        Name: Lock.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_LOCK_H__
#define __MXCORE_LOCK_H__

namespace mxcore
{

class Lock
{
private:
	Lock(const Lock&);
	Lock& operator =(const Lock&);
public:
	class ScopeLock
	{
	private:
		ScopeLock(const ScopeLock&);
		ScopeLock& operator=(const ScopeLock&);
	public:
		ScopeLock(Lock& lock);
		~ScopeLock(void);
	private:
		Lock& lock_;
	};

	Lock(void);
	virtual ~Lock(void);

	virtual void lock(void) = 0;
	virtual bool tryLock(void) = 0;
	virtual void unlock(void) = 0;
};

class FakeLock: public Lock
{
private:
	FakeLock(FakeLock&);
	FakeLock& operator =(const FakeLock&);
public:
	FakeLock(void);
	virtual ~FakeLock(void);

	virtual void lock(void);
	virtual bool tryLock(void);
	virtual void unlock(void);
};

} // namespace mxcore

#endif /* __MXCORE_LOCK_H__ */
