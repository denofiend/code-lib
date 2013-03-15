/*
 *        Name: SocketAddressTest.h
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef SOCKETADDRESSTEST_H_
#define SOCKETADDRESSTEST_H_

namespace mxcore
{

class SocketAddressTest
{
public:
	SocketAddressTest();
	virtual ~SocketAddressTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* SOCKETADDRESSTEST_H_ */
