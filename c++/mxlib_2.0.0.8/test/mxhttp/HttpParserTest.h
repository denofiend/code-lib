/*
 *        Name: HttpParserTest.h
 *
 *  Created on: 2011-2-24
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef HTTPPARSERTEST_H_
#define HTTPPARSERTEST_H_

namespace mxhttp
{

class HttpParserTest
{
public:
	HttpParserTest();
	virtual ~HttpParserTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* HTTPPARSERTEST_H_ */
