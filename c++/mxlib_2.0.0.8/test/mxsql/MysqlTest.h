/*
 *        Name: MysqlTest.h
 *
 *  Created on: 2011-2-1
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef MYSQLTEST_H_
#define MYSQLTEST_H_

namespace mxsql
{

class MysqlTest
{
public:
	MysqlTest();
	virtual ~MysqlTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

} // namespace mxsql

#endif /* MYSQLTEST_H_ */
