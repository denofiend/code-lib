/*
 *        Name: SystemOps.h
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXOS_SYSTEMOPS_H__
#define __MXOS_SYSTEMOPS_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mxcore/Assert.h>

#if defined(WIN32)
#include <windows.h>
#else
#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>
#endif

namespace mxos
{

inline int getLastError(void)
{
#if defined(WIN32)
	return ::GetLastError();
#else
	return errno;
#endif
}

inline const char* getErrorMsg(int errNo)
{
#if defined(WIN32)
	return "";
#else
	return ::strerror(errNo);
#endif
}
inline bool isFileExists(const char* filepath)
{
	int mode = 0;
	MX_ASSERT(NULL != filepath);

#ifdef WIN32
	mode = _A_RDONLY;
#else
	mode = R_OK;
#endif
	return 0 == ::access(filepath, mode);
}

inline bool isDirExists(const char* path)
{
	int mode = 0;
	MX_ASSERT(NULL != path);

#ifdef WIN32
	mode = _A_RDONLY;
#else
	mode = R_OK;
#endif
	struct stat st;
	::memset(&st, 0, sizeof(st));

	int err = ::stat(path, &st);
	if (0 != err)
	{
		return false;
	}

	return S_ISDIR(st.st_mode);
}

inline int64_t getFileSize(const char* filepath)
{
	struct stat st;

	MX_ASSERT(NULL != filepath);

	if (0 != ::stat(filepath, &st))
	{
		return (int64_t) -1;
	}

	if (!S_ISREG(st.st_mode))
	{
		return (int64_t) -1;
	}

	return (int64_t) st.st_size;
}

inline long getPid(void)
{
#if defined(WIN32)
	return ::GetCurrentProcessId();
#else
	return (long) ::getpid();
#endif
}

inline uint32_t getUid(const char* username)
{
#if defined(WIN32)
	return 0;
#else
	if (NULL == username)
	{
		return ::getuid();
	}
	else
	{
		struct passwd* pw = ::getpwnam(username);

		if (NULL == pw)
		{
			return (uint32_t) -1;
		}

		return pw->pw_uid;
	}
#endif
}

inline uint32_t getProcesserNum(void)
{
#if defined(WIN32)
	// TODO
	return 1;
#else
	return (uint32_t) ::sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

inline void daemonInit(const char* pname, const char* workDir, int facility)
{
#if !defined(WIN32)
	int i;
	pid_t pid;

	if ((pid = ::fork()) < 0)
	{
		return;
	}
	else if (pid)
	{
		::_exit(0); /* parent terminates */
	}

	/* child 1 continues... */

	if (setsid() < 0) /* become session leader */
	{
		return;
	}

	::signal(SIGHUP, SIG_IGN);

	if ((pid = ::fork()) < 0)
	{
		return;
	}
	else if (pid)
	{
		::_exit(0); /* child 1 terminates */
	}

	/* child 2 continues... */
	::chdir(workDir); /* change working directory */

	/* close off file descriptors */
	for (i = 0; i < 65535; i++)
	{
		::close(i);
	}

	/* redirect stdin, stdout, and stderr to /dev/null */
	::open("/dev/null", O_RDONLY);
	::open("/dev/null", O_RDWR);
	::open("/dev/null", O_RDWR);
#endif
}

} // namespace mxos

#endif /* __MXOS_SYSTEMOPS_H__ */
