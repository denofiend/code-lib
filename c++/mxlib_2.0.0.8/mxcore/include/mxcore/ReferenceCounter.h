/*
 *        Name: ReferenceCounter.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_REFERENCECOUNTER_H__
#define __MXCORE_REFERENCECOUNTER_H__

#include <mxcore/Assert.h>
#include <mxcore/Counter.h>
#include <string>
#include <mxcore/MemRecord.h>

namespace mxcore
{

template<class T, class CounterType = Counter>
class ReferenceCounter
{
public:
	ReferenceCounter(T* ptr) :
		ptr_(ptr), counter_(1)
	{
		MX_ASSERT(NULL != ptr_);
	}

	~ReferenceCounter(void)
	{
		if (NULL != ptr_)
		{
			delete ptr_;
			ptr_ = NULL;
		}
	}

	int32_t increment(void)
	{
		return counter_.increment();
	}

	int32_t decrement(void)
	{
		return counter_.decrement();
	}

	T* getPtr(void) const
	{
		return ptr_;
	}
private:
	T* ptr_;
	CounterType counter_;
};

template<class T, class CounterType = Counter>
class ReferenceArrayCounter
{
public:
	ReferenceArrayCounter(T* ptr) :
		ptr_(ptr), counter_(1)
	{
		MX_ASSERT(NULL != ptr_);
	}

	~ReferenceArrayCounter(void)
	{
		if (NULL != ptr_)
		{
			delete[] ptr_;
			ptr_ = NULL;
		}
	}

	int32_t increment(void)
	{
		return counter_.increment();
	}

	int32_t decrement(void)
	{
		return counter_.decrement();
	}

	T* getPtr(void) const
	{
		return ptr_;
	}
private:
	T* ptr_;
	CounterType counter_;
};

} // namespace mxcore

#endif /* __MXCORE_REFERENCECOUNTER_H__ */
