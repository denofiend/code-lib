/*
 * Memory_.h
 *
 *  Created on: 2009-9-25
 *      Author: zjg
 */

#ifndef __MXCORE__MEMORY__H__
#define __MXCORE__MEMORY__H__

#ifdef MEM_DEBUG

#include <new>
#include <memory>
/* Prototypes */
bool check_leaks();

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);
//void* operator new(size_t, const std::bad_alloc&);
//void* operator new[](size_t, const std::bad_alloc&);

void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, const char* file, int line);
void operator delete[](void* ptr, const char* file, int line);
void operator delete(void*, const std::bad_alloc&);
void operator delete[](void*, const std::bad_alloc&);

#define DEBUG_NEW new(__FILE__, __LINE__)
#define debug_new new(__FILE__, __LINE__)

#ifdef DEBUG_NEW_EMULATE_MALLOC
#include <stdlib.h>
#define malloc(s) ((void*)(debug_new char[s]))
#define free(p) delete[] (char*)(p)
#endif // DEBUG_NEW_EMULATE_MALLOC
/* Control flags */
extern bool new_verbose_flag; // default to false: no verbose information
extern bool new_autocheck_flag; // default to true: call check_leaks() on exit

#endif

#endif /* __MXCORE__MEMORY__H__ */
