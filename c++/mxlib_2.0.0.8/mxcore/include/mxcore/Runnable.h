/*
 *        Name: Runnable.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_RUNNABLE_H__
#define __MXCORE_RUNNABLE_H__

namespace mxcore
{

template<typename ReturnType>
class RunnableT
{
public:
	virtual ~RunnableT(void)
	{
	}

	virtual ReturnType run(void) = 0;
};

typedef RunnableT<void> Runnable;

} // namespace mxcore

#endif /* __MXCORE_RUNNABLE_H__ */
