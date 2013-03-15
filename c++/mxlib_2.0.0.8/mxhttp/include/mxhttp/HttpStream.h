/*
 *        Name: HttpStream.h
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPSTREAM_H__
#define __MXHTTP_HTTPSTREAM_H__

#include <string>
#include <stdint.h>
#include <mxcore/IOException.h>

namespace mxhttp
{

class HttpInputStream
{
public:
	virtual ~HttpInputStream(void)
	{
	}

	virtual int read(void* buf, int len) throw (mxcore::IOException&)= 0;
};

class HttpOutputStream
{
public:
	virtual ~HttpOutputStream(void)
	{
	}

	virtual int write(const void* buf, int len) throw (mxcore::IOException&)= 0;
};

class HttpStream: public HttpInputStream, public HttpOutputStream
{
public:
	enum
	{
		eWriting, eReading, eClosed
	};

	virtual ~HttpStream(void)
	{
	}

	virtual void close(void) = 0;
	virtual bool isClosed(void) const = 0;

	virtual void flip(void) = 0;
	virtual int64_t getContentLength(void) const = 0;

	virtual bool isInFile(void) const;
	virtual bool isInMemory(void) const;

	virtual int64_t available(void) const = 0;
};

class HttpStreamInMemory: public HttpStream
{
private:
	HttpStreamInMemory(const HttpStreamInMemory&);
	HttpStreamInMemory& operator =(const HttpStreamInMemory&);
public:
	HttpStreamInMemory(const std::string& content);
	virtual ~HttpStreamInMemory(void);

	virtual void close(void);
	virtual bool isClosed(void) const;

	virtual void flip(void);
	virtual int64_t getContentLength(void) const;

	virtual int read(void* buf, int len) throw (mxcore::IOException&);
	virtual int write(const void* buf, int len) throw (mxcore::IOException&);
	virtual bool isInMemory(void) const;

	virtual int64_t available(void) const;

	const std::string& getContent(void) const;
private:
	std::string content_;

	int64_t writePos_;
	int64_t readPos_;

	int state_;
};

inline 	const std::string& HttpStreamInMemory::getContent(void) const
{
	return content_;
}

class HttpStreamInfile: public HttpStream
{
private:
	HttpStreamInfile(const HttpStreamInfile&);
	HttpStreamInfile& operator =(const HttpStreamInfile&);
public:
	HttpStreamInfile(FILE* stream);
	virtual ~HttpStreamInfile(void);

	virtual void close(void);
	virtual bool isClosed(void) const;

	virtual void flip(void);
	virtual int64_t getContentLength(void) const;

	virtual int read(void* buf, int len) throw (mxcore::IOException&);
	virtual int write(const void* buf, int len) throw (mxcore::IOException&);
	virtual bool isInFile(void) const;

	virtual int64_t available(void) const;
private:
	FILE* stream_;

	int64_t writePos_;
	int64_t readPos_;

	int state_;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPSTREAM_H__ */
