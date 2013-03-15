/*
 *        Name: SocketTest.h
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef SOCKETTEST_H_
#define SOCKETTEST_H_

namespace mxcore
{

class SocketTest
{
public:
	SocketTest();
	virtual ~SocketTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* SOCKETTEST_H_ */
