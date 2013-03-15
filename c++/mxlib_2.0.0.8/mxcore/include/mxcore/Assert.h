/*
 *        Name: Assert.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_ASSERT_H__
#define __MXCORE_ASSERT_H__

#include <stdio.h>
#include <stdlib.h>

#define MX_ASSERT(x) \
	do { \
		if (!(x)) { \
			fprintf(stderr, "assertion "#x" failed in %s:%d\n", __FILE__, __LINE__); \
			fflush(stderr); \
			abort(); \
		} \
	} while (0)

#endif /* __MXCORE_ASSERT_H__ */
