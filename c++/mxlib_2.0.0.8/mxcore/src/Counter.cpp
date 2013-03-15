/*
 *        Name: Counter.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Counter.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

Counter::Counter(int32_t initValue) :
	atom_((mxos::AtomValueT) initValue)
{
}

Counter::~Counter(void)
{
}

int32_t Counter::increment(void)
{
	return mxos::atomIncrement(atom_);
}

int32_t Counter::decrement(void)
{
	return mxos::atomDecrement(atom_);
}

int32_t Counter::getCount(void) const
{
	return atom_.value_;
}

} // namespace mxcore
