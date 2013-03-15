/*
 *        Name: ThreadOps.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXOS_THREADOPS_H__
#define __MXOS_THREADOPS_H__

#include <stdint.h>
#include <limits.h>

#if defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#include <mxcore/Assert.h>

namespace mxos
{

typedef void* (*ThreadProc)(void* param);

#if defined(WIN32)
typedef CRITICAL_SECTION MutexT;

typedef struct ConditionT
{
	ConditionT() :
		semaphore_(NULL), wait_count_(0)
	{
	}

	HANDLE semaphore_;
	LONG wait_count_;
} ConditionT;

typedef struct ThreadT
{
	ThreadT() :
		handle_(NULL), thread_id_(0)
	{
	}

	HANDLE handle_;
	ULONG thread_id_;
} ThreadT;

typedef LONG AtomValueT;

typedef struct AtomT
{
	AtomT(AtomValueT value = 0) :
		value_(value)
	{
	}
	AtomValueT value_;
} AtomT;
#else

typedef pthread_mutex_t MutexT;
typedef pthread_cond_t ConditionT;

typedef struct ThreadT
{
	ThreadT():
	thread_(0)
	{}

	pthread_t thread_;
}ThreadT;

typedef int32_t AtomValueT;

typedef struct AtomT
{
	AtomT(AtomValueT value = 0) :
	value_(value)
	{
		pthread_mutexattr_t attr;
		::pthread_mutexattr_init(&attr);
		::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		::pthread_mutex_init(&mutex_, &attr);
		::pthread_mutexattr_destroy(&attr);
	}

	~AtomT(void)
	{
		::pthread_mutex_destroy(&mutex_);
	}
	AtomValueT value_;
	MutexT mutex_;
} AtomT;
#endif

inline void mutexInit(MutexT& mutex)
{
#if defined(WIN32)
	::InitializeCriticalSection(&mutex);
#else
	pthread_mutexattr_t attr;
	::pthread_mutexattr_init(&attr);
	::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	::pthread_mutex_init(&mutex, &attr);
	::pthread_mutexattr_destroy(&attr);
#endif
}

inline void mutexDestroy(MutexT& mutex)
{
#if defined(WIN32)
	::DeleteCriticalSection(&mutex);
#else
	::pthread_mutex_destroy(&mutex);
#endif
}

inline bool mutexTryLock(MutexT& mutex)
{
#if defined(WIN32)
	return ::TryEnterCriticalSection(&mutex);
#else
	return ::pthread_mutex_trylock(&mutex) != 0;
#endif
}

inline void mutexLock(MutexT& mutex)
{
#if defined(WIN32)
	::EnterCriticalSection(&mutex);
#else
	::pthread_mutex_lock(&mutex);
#endif
}

inline void mutexUnlock(MutexT& mutex)
{
#if defined(WIN32)
	return ::LeaveCriticalSection(&mutex);
#else
	::pthread_mutex_unlock(&mutex);
#endif
}

inline void conditionInit(ConditionT& condition)
{
#if defined(WIN32)
	condition.semaphore_ = ::CreateSemaphore(NULL, // default security attributes - lpSemaphoreAttributes是信号量的安全属性
			0, // initial getCount - lInitialCount是初始化的信号量
			LONG_MAX, // maximum getCount - lMaximumCount是允许信号量增加到最大值
			NULL); // unnamed semaphore
	condition.wait_count_ = 0;
#else
	::pthread_cond_init(&condition, NULL);
#endif
}

inline void conditionDestroy(ConditionT& condition)
{
#if defined(WIN32)
	if (NULL != condition.semaphore_)
	{
		::CloseHandle(condition.semaphore_);
		condition.semaphore_ = NULL;
		condition.wait_count_ = 0;
	}
#else
	::pthread_cond_destroy(&condition);
#endif
}

inline void conditionNotify(ConditionT& condition)
{
#if defined(WIN32)
	MX_ASSERT(NULL != condition.semaphore_);

	if (::InterlockedDecrement(&condition.wait_count_))
	{
		::InterlockedIncrement(&condition.wait_count_);
	}

	::ReleaseSemaphore(condition.semaphore_, 1, NULL);
#else
	::pthread_cond_signal(&condition);
#endif
}

inline void conditionNotifyAll(ConditionT& condition)
{
#if defined(WIN32)
	MX_ASSERT(NULL != condition.semaphore_);

	LONG count = ::InterlockedExchange(&condition.wait_count_, 0);

	::ReleaseSemaphore(condition.semaphore_, count, NULL);
#else
	::pthread_cond_broadcast(&condition);
#endif
}

inline void conditionWait(ConditionT& condition, MutexT& mutex, long timeout)
{
#if defined(WIN32)
	MX_ASSERT(NULL != condition.semaphore_);

	::InterlockedIncrement(&condition.wait_count_);

	mutexUnlock(mutex);
	if (timeout < 0)
	{
		timeout = INFINITE;
	}
	::WaitForSingleObject(condition.semaphore_, timeout);
	mutexLock(mutex);
#else
	if (timeout < 0)
	{
		::pthread_cond_wait(&condition, &mutex);
	}
	else
	{
		struct timespec ts;
		struct timeval tv;
		struct timezone tz;

		::gettimeofday(&tv, &tz);

		long seconds = timeout / 1000;

		tv.tv_sec += seconds;
		tv.tv_usec += (timeout - seconds * 1000) * 1000;

		while (tv.tv_usec > 1000000)
		{
			tv.tv_sec++;
			tv.tv_usec -= 1000000;
		}

		ts.tv_sec = tv.tv_sec;
		ts.tv_nsec = tv.tv_usec * 1000;

		::pthread_cond_timedwait(&condition, &mutex, &ts);
	}
#endif
}

inline int threadStart(ThreadT& thread, ThreadProc proc, void* param)
{
#if defined(WIN32)
	thread.handle_ = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) proc,
			param, 0/*CREATE_SUSPENDED*/, &thread.thread_id_);

	if (NULL == thread.handle_)
	{
		return -1;
	}
#else
	if (0 != ::pthread_create(&thread.thread_, NULL, proc, param))
	{
		return -1;
	}
#endif
	return 0;
}

inline void threadJoin(ThreadT& thread)
{
#if defined(WIN32)
	MX_ASSERT(NULL != thread.handle_);
	::WaitForSingleObject(thread.handle_, INFINITE);
	::CloseHandle(thread.handle_);

	thread.handle_ = NULL;
	thread.thread_id_ = 0;
#else
	MX_ASSERT(0 != thread.thread_);
	::pthread_join(thread.thread_, NULL);
	thread.thread_ = 0;
#endif
}

inline long threadSelf(const ThreadT& thread)
{
#if defined(WIN32)
	MX_ASSERT(NULL != thread.handle_);
	return thread.thread_id_;
#else
	MX_ASSERT(0 != thread.thread_);
	return (long)thread.thread_;
#endif
}

inline long threadCurrent(void)
{
#if defined(WIN32)
	return (long) ::GetCurrentThreadId();
#else
	return (long)::pthread_self();
#endif
}

inline void threadSleep(long timeout)
{
#if defined(WIN32)
	::Sleep(timeout);
#else
	struct timeval tv;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	::select(0, NULL, NULL, NULL, &tv);
#endif
}

#if defined(WIN32)
inline AtomValueT atomGet(AtomT& destination)
{
	return destination.value_;
}

inline AtomValueT atomCompareExchange(AtomT& destination, AtomValueT exchange,
		AtomValueT comperand)
{
	return ::InterlockedCompareExchange(&destination.value_, exchange,
			comperand);
}

inline AtomValueT atomExchange(AtomT& destination, AtomValueT exchange)
{
	return ::InterlockedExchange(&destination.value_, exchange);
}

inline AtomValueT atomAdd(AtomT& destination, AtomValueT add)
{
	for (;;)
	{
		AtomValueT old_value = destination.value_;

		if (old_value == ::InterlockedCompareExchange(&destination.value_,
				old_value + add, old_value))
		{
			return old_value + add;
		}
	}
}

inline AtomValueT atomIncrement(AtomT& destination)
{
	return ::InterlockedIncrement(&destination.value_);
}

inline AtomValueT atomDecrement(AtomT& destination)
{
	return ::InterlockedDecrement(&destination.value_);
}
#else
inline AtomValueT atomGet(AtomT& destination)
{
	return destination.value_;
}

inline AtomValueT atomCompareExchange(AtomT& destination, AtomValueT exchange,
		AtomValueT comperand)
{
	AtomValueT old;

	mutexLock(destination.mutex_);
	old = destination.value_;
	if (comperand == destination.value_)
	{
		destination.value_ = exchange;
	}
	mutexUnlock(destination.mutex_);
	return old;
}

inline AtomValueT atomExchange(AtomT& destination, AtomValueT exchange)
{
	AtomValueT old;
	mutexLock(destination.mutex_);
	old = destination.value_;
	destination.value_ = exchange;
	mutexUnlock(destination.mutex_);

	return old;
}

inline AtomValueT atomAdd(AtomT& destination, AtomValueT add)
{
	AtomValueT ret;
	mutexLock(destination.mutex_);
	destination.value_ += add;
	ret = destination.value_;
	mutexUnlock(destination.mutex_);
	return ret;
}

inline AtomValueT atomIncrement(AtomT& destination)
{
	AtomValueT ret;
	mutexLock(destination.mutex_);
	destination.value_ ++;
	ret = destination.value_;
	mutexUnlock(destination.mutex_);
	return ret;
}

inline AtomValueT atomDecrement(AtomT& destination)
{
	AtomValueT ret;
	mutexLock(destination.mutex_);
	destination.value_--;
	ret = destination.value_;
	mutexUnlock(destination.mutex_);
	return ret;
}
#endif

} // namespace mxos

#endif /* __MXOS_THREADOPS_H__ */
