/*
 * HttpVirtualServerTest.h
 *
 *  Created on: 2011-3-9
 *      Author: zhaojiangang
 */

#ifndef HTTPVIRTUALSERVERTEST_H_
#define HTTPVIRTUALSERVERTEST_H_

namespace mxhttp
{

class HttpVirtualServerTest
{
public:
	HttpVirtualServerTest();
	virtual ~HttpVirtualServerTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* HTTPVIRTUALSERVERTEST_H_ */
