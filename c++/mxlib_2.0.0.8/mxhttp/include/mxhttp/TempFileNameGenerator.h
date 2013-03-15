/*
 *        Name: TempFileNameGenerator.h
 *
 *  Created on: 2011-2-14
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_TEMPFILENAMEGENERATOR_H__
#define __MXHTTP_TEMPFILENAMEGENERATOR_H__

#include <string>
#include <mxcore/Counter.h>

namespace mxhttp
{

class TempFileNameGenerator
{
public:
	virtual ~TempFileNameGenerator(void)
	{
	}

	virtual std::string getNextFileName(void) = 0;
};

class TempFileNameGeneratorSimple: public TempFileNameGenerator
{
public:
	TempFileNameGeneratorSimple(const std::string& rootPath);

	virtual ~TempFileNameGeneratorSimple(void);

	virtual std::string getNextFileName(void);
private:
	mxcore::Counter fileNo_;
	std::string rootPath_;
};

} // namespace mxhttp

#endif /* __MXHTTP_TEMPFILENAMEGENERATOR_H__ */
