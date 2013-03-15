/*
 *        Name: Atom.h
 *
 *  Created on: 2011-2-16
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_ATOM_H__
#define __MXCORE_ATOM_H__

#include <mxcore/ThreadOps.h>

namespace mxcore
{

template<class T, T defaultValue>
class Atom
{
public:
	Atom(void) :
		value_((mxos::AtomValueT) defaultValue)
	{
	}

	Atom(T value) :
		value_((mxos::AtomValueT) value)
	{
	}

	~Atom(void)
	{
	}

	void set(T value)
	{
		mxos::atomExchange(value_, (mxos::AtomValueT) value);
	}

	T get(void)
	{
		return (T) mxos::atomGet(value_);
	}

	T getAndSet(T value)
	{
		return (T) mxos::atomExchange(value_, (mxos::AtomValueT) value);
	}
private:
	mxos::AtomT value_;
};

typedef Atom<bool, false> AtomBool;

} // namespace mxcore

#endif /* __MXCORE_ATOM_H__ */
