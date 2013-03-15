/*
 *        Name: ScopePtr.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_SCOPEPTR_H__
#define __MXCORE_SCOPEPTR_H__

#include <mxcore/Assert.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

template<class T>
class ScopePtr
{
public:
	explicit ScopePtr(T* ptr = NULL) :
		ptr_(ptr)
	{
	}

	explicit ScopePtr(ScopePtr& src)
	{
		ptr_ = src.detach();
	}

	~ScopePtr(void)
	{
		release();
	}

	ScopePtr& operator =(ScopePtr& other)
	{
		if (ptr_ != other.ptr_)
		{
			release();
			ptr_ = other.detach();
		}

		return *this;
	}

	ScopePtr& operator =(T* ptr)
	{
		if (ptr_ != ptr)
		{
			release();
			ptr_ = ptr;
		}

		return *this;
	}

	void release(void)
	{
		if (NULL != ptr_)
		{
			delete ptr_;
			ptr_ = NULL;
		}
	}

	bool isNull(void) const
	{
		return NULL == ptr_;
	}

	T* detach(void)
	{
		T* ptr = ptr_;
		ptr_ = NULL;
		return ptr;
	}

	void attach(T* ptr)
	{
		release();
		ptr_ = ptr;
	}

	T* pointer(void) const
	{
		return ptr_;
	}

	T& operator *(void)
	{
		MX_ASSERT(NULL != ptr_);
		return *ptr_;
	}

	const T& operator *(void) const
	{
		MX_ASSERT(NULL != ptr_);
		return *ptr_;
	}

	T** operator &(void)
	{
		MX_ASSERT(NULL == ptr_);
		return &ptr_;
	}

	T* operator ->(void) const
	{
		MX_ASSERT(NULL != ptr_);
		return ptr_;
	}
public:
	T* ptr_;
};

template<class T>
class ScopeArrayPtr
{
public:
	explicit ScopeArrayPtr(T* ptr = NULL) :
		ptr_(ptr)
	{
	}

	explicit ScopeArrayPtr(ScopeArrayPtr& src)
	{
		ptr_ = src.detach();
	}

	~ScopeArrayPtr(void)
	{
		release();
	}

	ScopeArrayPtr& operator =(ScopeArrayPtr& other)
	{
		if (ptr_ != other.ptr_)
		{
			release();
			ptr_ = other.detach();
		}

		return *this;
	}

	ScopeArrayPtr& operator =(T* ptr)
	{
		if (ptr_ != ptr)
		{
			release();
			ptr_ = ptr;
		}

		return *this;
	}

	void release(void)
	{
		if (NULL != ptr_)
		{
			delete[] ptr_;
			ptr_ = NULL;
		}
	}

	bool isNull(void) const
	{
		return NULL == ptr_;
	}

	T* detach(void)
	{
		T* ptr = ptr_;
		ptr_ = NULL;
		return ptr;
	}

	void attach(T* ptr)
	{
		release();
		ptr_ = ptr;
	}

	T* pointer(void) const
	{
		return ptr_;
	}

	T& operator *(void)
	{
		MX_ASSERT(NULL != ptr_);
		return *ptr_;
	}

	const T& operator *(void) const
	{
		MX_ASSERT(NULL != ptr_);
		return *ptr_;
	}

	T** operator &(void)
	{
		MX_ASSERT(NULL == ptr_);
		return &ptr_;
	}

	T& operator[](int index)
	{
		MX_ASSERT(NULL != ptr_);
		return ptr_[index];
	}

	const T& operator[](int index) const
	{
		MX_ASSERT(NULL != ptr_);
		return ptr_[index];
	}
public:
	T* ptr_;
};

} // namespace mxcore

#endif /* __MXCORE_SCOPEPTR_H__ */
