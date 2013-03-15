/*
 *        Name: SharedPtr.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_SHAREDPTR_H__
#define __MXCORE_SHAREDPTR_H__

#include <mxcore/Assert.h>
#include <mxcore/ReferenceCounter.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

template<class T, class CounterType>
class SharedPtrT
{
public:
	SharedPtrT(T* ptr = NULL) :
		counter_(NULL)
	{
		if (NULL != ptr)
		{
			counter_ = new CounterType(ptr);
		}
	}

	SharedPtrT(const SharedPtrT& src) :
		counter_(src.counter_)
	{
		if (NULL != counter_)
		{
			counter_->increment();
		}
	}

	friend bool operator ==(const SharedPtrT& sp1, const SharedPtrT& sp2)
	{
		if (sp1.counter_ == sp2.counter_)
		{
			return true;
		}

		if (NULL == sp1.counter_ || NULL == sp2.counter_)
		{
			return false;
		}
		return sp1.counter_->getPtr() == sp2.counter_->getPtr();
	}

	friend bool operator !=(const SharedPtrT& sp1, const SharedPtrT& sp2)
	{
		if (sp1.counter_ == sp2.counter_)
		{
			return false;
		}

		if (NULL == sp1.counter_ || NULL == sp2.counter_)
		{
			return true;
		}

		return sp1.counter_->getPtr() != sp2.counter_->getPtr();
	}

	friend bool operator >(const SharedPtrT& sp1, const SharedPtrT& sp2)
	{
		if (sp1.counter_ == sp2.counter_)
		{
			return false;
		}

		if (NULL == sp1.counter_)
		{
			return false;
		}

		if (NULL == sp2.counter_)
		{
			return true;
		}

		return sp1.counter_->getPtr() > sp2.counter_->getPtr();
	}

	friend bool operator <(const SharedPtrT& sp1, const SharedPtrT& sp2)
	{
		if (sp1.counter_ == sp2.counter_)
		{
			return false;
		}

		if (NULL == sp1.counter_)
		{
			return true;
		}

		if (NULL == sp2.counter_)
		{
			return false;
		}

		return sp1.counter_->getPtr() < sp2.counter_->getPtr();
	}

	SharedPtrT& operator =(T* ptr)
	{
		if (NULL != counter_ && counter_->getPtr() == ptr)
		{
			return *this;
		}

		release();

		if (NULL != ptr)
		{
			counter_ = new CounterType(ptr);
		}

		return *this;
	}

	SharedPtrT& operator =(const SharedPtrT& other)
	{
		if (counter_ != other.counter_)
		{
			release();

			if (NULL != other.counter_)
			{
				counter_ = other.counter_;
				counter_->increment();
			}
		}

		return *this;
	}

	~SharedPtrT(void)
	{
		release();
	}

	bool isNull(void) const
	{
		return NULL == counter_;
	}

	T* pointer(void) const
	{
		return (NULL != counter_) ? counter_->getPtr() : NULL;
	}

	T* operator ->(void) const
	{
		MX_ASSERT(NULL != counter_);
		return counter_->getPtr();
	}

	T& operator *(void)
	{
		MX_ASSERT(NULL != counter_);
		return *counter_->getPtr();
	}

	const T& operator *(void) const
	{
		MX_ASSERT(NULL != counter_);
		return *counter_->getPtr();
	}

	void release(void)
	{
		if (NULL != counter_ && 0 == counter_->decrement())
		{
			delete counter_;
		}

		counter_ = NULL;
	}
private:
	CounterType* counter_;
};

template<class T>
class SharedPtr: public SharedPtrT<T, ReferenceCounter<T> >
{
	typedef SharedPtrT<T, ReferenceCounter<T> > baseClass;
public:
	SharedPtr(T* ptr = NULL) :
		baseClass(ptr)
	{
	}

	SharedPtr(const SharedPtr& src) :
		baseClass(src)
	{
	}

	SharedPtr& operator =(T* ptr)
	{
		baseClass::operator =(ptr);
		return *this;
	}

	SharedPtr& operator =(const SharedPtr& other)
	{
		baseClass::operator =(other);
		return *this;
	}

	~SharedPtr(void)
	{
	}
};

template<class T>
class SharedArrayPtr: public SharedPtrT<T, ReferenceCounter<T> >
{
	typedef SharedPtrT<T, ReferenceCounter<T> > baseClass;
public:
	SharedArrayPtr(T* ptr = NULL) :
		baseClass(ptr)
	{
	}

	SharedArrayPtr(const SharedArrayPtr& src) :
		baseClass(src)
	{
	}

	SharedArrayPtr& operator =(T* ptr)
	{
		baseClass::operator =(ptr);
		return *this;
	}

	SharedArrayPtr& operator =(const SharedArrayPtr& other)
	{
		baseClass::operator =(other);
		return *this;
	}

	~SharedArrayPtr(void)
	{
	}
};

} // namespace mxcore

#endif /* __MXCORE_SHAREDPTR_H__ */
