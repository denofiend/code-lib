/*
 * HttpServerTest.h
 *
 *  Created on: 2011-3-3
 *      Author: zhaojiangang
 */

#ifndef HTTPSERVERTEST_H_
#define HTTPSERVERTEST_H_

namespace mxhttp
{

class HttpServerTest
{
public:
	HttpServerTest();
	virtual ~HttpServerTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* HTTPSERVERTEST_H_ */
