/*
 *        Name: Verify.h
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_VERIFY_H__
#define __MXCORE_VERIFY_H__

#include <stdio.h>

#define MX_VERIFY(x) \
	do { \
		if (!(x)) { \
			::fprintf(stderr, "the test case ["#x"] failed in %s:%d\n", __FILE__, __LINE__); \
			::fflush(stderr); \
			::abort(); \
		} \
	} while (0)

#endif /* __MXCORE_VERIFY_H__ */
