/*
 * DataSourceTest.h
 *
 *  Created on: 2011-4-28
 *      Author: zhaojiangang
 */

#ifndef DATASOURCETEST_H_
#define DATASOURCETEST_H_

namespace mxsql
{

class DataSourceTest
{
public:
	DataSourceTest();
	virtual ~DataSourceTest();

	virtual void init(void);
	virtual void perform(void);
	virtual void cleanup(void);
};

}

#endif /* DATASOURCETEST_H_ */
