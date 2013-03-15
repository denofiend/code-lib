/*
 *        Name: HttpBodyTest.h
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef HTTPBODYTEST_H_
#define HTTPBODYTEST_H_

namespace mxhttp
{

class HttpBodyTest
{
public:
	HttpBodyTest();
	virtual ~HttpBodyTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* HTTPBODYTEST_H_ */
