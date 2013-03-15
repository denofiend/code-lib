/*
 *        Name: Counter.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_COUNTER_H__
#define __MXCORE_COUNTER_H__

#include <stdint.h>
#include <mxcore/ThreadOps.h>

namespace mxcore
{

class Counter
{
public:
	Counter(int32_t initValue = 1);
	~Counter(void);

	int32_t increment(void);
	int32_t decrement(void);
	int32_t getCount(void) const;
private:
	mxos::AtomT atom_;
};

} // namespace mxcore

#endif /* __MXCORE_COUNTER_H__ */
