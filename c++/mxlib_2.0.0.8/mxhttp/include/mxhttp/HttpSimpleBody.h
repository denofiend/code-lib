/*
 *        Name: HttpSimpleBody.h
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPSIMPLEBODY_H__
#define __MXHTTP_HTTPSIMPLEBODY_H__

#include <mxhttp/HttpBody.h>
#include <mxhttp/HttpStream.h>

namespace mxhttp
{

class HttpSimpleBody: public HttpBody
{
private:
	HttpSimpleBody(const HttpSimpleBody&);
	HttpSimpleBody& operator =(const HttpSimpleBody&);
protected:
	enum
	{
		eInMemory, eInFile
	};

	HttpSimpleBody(int type, HttpStream* stream);
public:
	virtual ~HttpSimpleBody(void);

	virtual bool isSimpleBody(void) const;

	void flip(void);
	int64_t getContentLength(void) const;

	int64_t available(void) const;

	int read(void* buf, int len) throw (mxcore::IOException&);
	int write(const void* buf, int len) throw (mxcore::IOException&);

	bool isInFile(void) const;
	bool isInMemory(void) const;

	void setContentType(const std::string& contentType);
	const std::string& getContentType(void) const;

	static HttpSimpleBody* makeInFile(const std::string& filepath, bool isTemp)
			throw (mxcore::IOException&);

	static HttpSimpleBody* makeInFile(HttpSimpleBody* srcBody,
			const std::string& filepath, bool isTemp)
			throw (mxcore::IOException&);

	static HttpSimpleBody* makeInMemory(const std::string& content);
protected:
	virtual void closeImpl(void);
protected:
	int type_;
	HttpStream* stream_;
	std::string contentType_;
};

inline bool HttpSimpleBody::isInFile(void) const
{
	return eInFile == type_;
}

inline bool HttpSimpleBody::isInMemory(void) const
{
	return eInMemory == type_;
}

inline void HttpSimpleBody::setContentType(const std::string& contentType)
{
	contentType_ = contentType;
}

inline const std::string& HttpSimpleBody::getContentType(void) const
{
	return contentType_;
}

class HttpSimpleBodyInMemory: public HttpSimpleBody
{
	friend class HttpSimpleBody;
private:
	HttpSimpleBodyInMemory(const HttpSimpleBodyInMemory&);
	HttpSimpleBodyInMemory& operator =(const HttpSimpleBodyInMemory&);

	HttpSimpleBodyInMemory(const std::string& content);
protected:
	virtual ~HttpSimpleBodyInMemory(void);
};

class HttpSimpleBodyInFile: public HttpSimpleBody
{
	friend class HttpSimpleBody;
private:
	HttpSimpleBodyInFile(const HttpSimpleBodyInFile&);
	HttpSimpleBodyInFile& operator =(const HttpSimpleBodyInFile&);

	HttpSimpleBodyInFile(FILE* stream, const std::string& filepath,
			bool removeFileOnClose);
public:
	virtual ~HttpSimpleBodyInFile(void);
protected:
	virtual void closeImpl(void);
private:
	std::string filepath_;
	bool removeFileOnClose_;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPSIMPLEBODY_H__ */
