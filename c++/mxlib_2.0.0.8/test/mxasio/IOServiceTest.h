/*
 *        Name: IOServiceTest.h
 *
 *  Created on: 2011-2-23
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef IOSERVICETEST_H_
#define IOSERVICETEST_H_

namespace mxasio
{

class IOServiceTest
{
public:
	IOServiceTest();
	virtual ~IOServiceTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* IOSERVICETEST_H_ */
