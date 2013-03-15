/*
 * Memory.cpp
 *
 *  Created on: Feb 12, 2009
 *      Author: zjg
 */

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef MEM_DEBUG

#ifndef DEBUG_NEW_HASHTABLESIZE
#define DEBUG_NEW_HASHTABLESIZE 16384
#endif

#ifndef DEBUG_NEW_HASH
#define DEBUG_NEW_HASH(p) ( ((unsigned long)(p) >> 8) % DEBUG_NEW_HASHTABLESIZE)
#endif

// The default behaviour now is to copy the file name, because we found
// that the exit leakage check cannot access the address of the file
// name sometimes (in our case, a core dump will occur when trying to
// access the file name in a shared library after a SIGINT).
#ifndef DEBUG_NEW_FILENAME_LEN
#define DEBUG_NEW_FILENAME_LEN    256
#endif

#if DEBUG_NEW_FILENAME_LEN == 0 && !defined(DEBUG_NEW_NO_FILENAME_COPY)
#define DEBUG_NEW_NO_FILENAME_COPY
#endif

#ifndef DEBUG_NEW_NO_FILENAME_COPY
#include <string.h>
#endif

typedef struct new_ptr_list_t new_ptr_list_t;

struct new_ptr_list_t
{
	new_ptr_list_t* next;
#ifdef DEBUG_NEW_NO_FILENAME_COPY
	const char* file;
#else
	char file[DEBUG_NEW_FILENAME_LEN];
#endif
	int line;
	size_t size;
	int array;
};

static new_ptr_list_t* new_ptr_list[DEBUG_NEW_HASHTABLESIZE];

bool new_verbose_flag = false;
bool new_autocheck_flag = true;

bool check_leaks()
{
	bool bLeaked = false;

	for (int i = 0; i < DEBUG_NEW_HASHTABLESIZE; ++i)
	{
		new_ptr_list_t* ptr = new_ptr_list[i];

		if (ptr == NULL)
			continue;

		bLeaked = true;

		while (ptr)
		{
			printf("Leaked object at %p (size %u, %s:%d)\n",
					(char*) ptr + sizeof(new_ptr_list_t),
					(unsigned int) ptr->size, ptr->file, ptr->line);
			ptr = ptr->next;
		}
	}

	return bLeaked;
}

#ifdef WIN32

class new_lock_t
{
public:
	new_lock_t()
	{
		InitializeCriticalSection(&_cs);
	}

	~new_lock_t()
	{
		DeleteCriticalSection(&_cs);
	}

	void lock()
	{
		EnterCriticalSection(&_cs);
	}

	void unlock()
	{
		LeaveCriticalSection(&_cs);
	}
private:
	CRITICAL_SECTION _cs;
};

#else

class new_lock_t
{
public:
	new_lock_t()
	{
		pthread_mutex_init(&new_lock, NULL);
	}

	~new_lock_t()
	{
		pthread_mutex_destroy(&new_lock);
	}

	void lock()
	{
		pthread_mutex_lock(&new_lock);
	}

	void unlock()
	{
		pthread_mutex_unlock(&new_lock);
	}
private:
	pthread_mutex_t new_lock;
};
#endif

static new_lock_t s_new_lock;

static void lock_hash_table(void)
{
	s_new_lock.lock();
}

static void unlock_hash_table(void)
{
	s_new_lock.unlock();
}

static void* inner_new(size_t size, const char* file, int line, int array)
{
	size_t s = size + sizeof(new_ptr_list_t);
	new_ptr_list_t* ptr = (new_ptr_list_t*) malloc(s);

	if (ptr == NULL)
	{
		fprintf(stderr, "new:  out of memory when allocating %u bytes\n",
				(unsigned int) size);
		abort();
	}

	lock_hash_table();

	void* pointer = (char*) ptr + sizeof(new_ptr_list_t);
	size_t hash_index = DEBUG_NEW_HASH(pointer);
	ptr->next = new_ptr_list[hash_index];

#ifdef DEBUG_NEW_NO_FILENAME_COPY
	ptr->file = file;
#else
	strncpy(ptr->file, file, DEBUG_NEW_FILENAME_LEN - 1);
	ptr->file[DEBUG_NEW_FILENAME_LEN - 1] = '\0';
#endif

	ptr->line = line;
	ptr->size = size;
	ptr->array = array;

	new_ptr_list[hash_index] = ptr;

	if (new_verbose_flag)
	{
		printf("new:  allocated  %p (size %u, %s:%d)\n", pointer,
				(unsigned int) size, file, line);
	}

	unlock_hash_table();

	return pointer;
}

void* operator new(size_t size)
{
	return inner_new(size, "<Unknown>", 0, 0);
}

void* operator new(size_t size, const char* file, int line)
{
	return inner_new(size, file, line, 0);
}

void* operator new[](size_t size, const char* file, int line)
{
	return inner_new(size, file, line, 1);
}

static void inner_delete(void* pointer, int array, const char* file, int line)
{
	if (pointer == NULL)
		return;

	size_t hash_index = DEBUG_NEW_HASH(pointer);

	lock_hash_table();

	new_ptr_list_t* ptr = new_ptr_list[hash_index];
	new_ptr_list_t* ptr_last = NULL;

	while (ptr)
	{
		if ((char*) ptr + sizeof(new_ptr_list_t) == pointer)
		{
			if (array != ptr->array)
			{
				if (ptr->array)
				{
					fprintf(
							stderr,
							"delete: you must call delete [] %p, %p allocated by %s:%d, size=%d\n",
							pointer, pointer, ptr->file, ptr->line,
							(int) ptr->size);

				}
				else
				{
					fprintf(
							stderr,
							"delete: you must call delete %p, %p allocated by %s:%d, size=%d\n",
							pointer, pointer, ptr->file, ptr->line,
							(int) ptr->size);
				}

				fflush(stderr);
			}

			if (new_verbose_flag)
			{
				printf("delete: freeing  %p (size %u)\n", pointer,
						(unsigned int) ptr->size);
			}

			if (ptr_last == NULL)
			{
				new_ptr_list[hash_index] = ptr->next;
			}
			else
			{
				ptr_last->next = ptr->next;
			}

			unlock_hash_table();

			free(ptr);

			return;
		}
		ptr_last = ptr;
		ptr = ptr->next;
	}

	unlock_hash_table();

	fprintf(stderr, "delete: invalid pointer %p\n", pointer);
	fflush(stderr);

	abort();
}

void operator delete(void* pointer)
{
	inner_delete(pointer, 0, "Unknown", 0);
}

void operator delete[](void* pointer)
{
	inner_delete(pointer, 1, "Unknown", 0);
}

void operator delete(void* ptr, const char* file, int line)
{
	inner_delete(ptr, 0, file, line);
}

void operator delete[](void* ptr, const char* file, int line)
{
	inner_delete(ptr, 1, file, line);
}

void operator delete(void* ptr, const std::bad_alloc&)
{
	inner_delete(ptr, 0, "Unknown", 0);
}

void operator delete[](void* ptr, const std::bad_alloc&)
{
	inner_delete(ptr, 1, "Unknown", 0);
}

// Proxy class to automatically call check_leaks if new_autocheck_flag is set
class new_check_t
{
public:
	new_check_t()
	{
	}
	~new_check_t()
	{
		if (new_autocheck_flag)
		{
			// Check for leakage.
			// If any leaks are found, set new_verbose_flag so that any
			// delete operations in the destruction of global/static
			// objects will display information to compensate for
			// possible false leakage reports.
			if (check_leaks())
				new_verbose_flag = true;
		}
	}
};

static new_check_t new_check_object;

#endif // MEM_DEBUG
