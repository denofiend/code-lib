/*
 * HttpSingleFileUploadServlet.h
 *
 *  Created on: 2011-3-8
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPSINGLEFILEUPLOADSERVLET_H__
#define __MXHTTP_HTTPSINGLEFILEUPLOADSERVLET_H__

#include <list>
#include <mxhttp/HttpServlet.h>

namespace mxhttp
{

class FilenamePartGenerator
{
public:
	virtual ~FilenamePartGenerator(void)
	{
	}

	virtual std::string genPart(const HttpRequest& request) = 0;
};

class TimePartGenerator: public FilenamePartGenerator
{
public:
	TimePartGenerator(const std::string& prefix);

	virtual ~TimePartGenerator(void);

	virtual std::string genPart(const HttpRequest& request);
private:
	std::string prefix_;
};

class PidPartGenerator: public FilenamePartGenerator
{
public:
	PidPartGenerator(const std::string& prefix);

	virtual ~PidPartGenerator(void);

	virtual std::string genPart(const HttpRequest& request);
private:
	std::string prefix_;
};

class ClientIpPartGenerator: public FilenamePartGenerator
{
public:
	ClientIpPartGenerator(const std::string& prefix);

	virtual ~ClientIpPartGenerator(void);

	virtual std::string genPart(const HttpRequest& request);
private:
	std::string prefix_;
};

class StrPartGenerator: public FilenamePartGenerator
{
public:
	StrPartGenerator(const std::string& prefix);

	virtual ~StrPartGenerator(void);

	virtual std::string genPart(const HttpRequest& request);
private:
	std::string prefix_;
};

class SeqPartGenerator: public FilenamePartGenerator
{
public:
	SeqPartGenerator(const std::string& prefix);

	virtual ~SeqPartGenerator(void);

	virtual std::string genPart(const HttpRequest& request);
private:
	std::string prefix_;
	unsigned int sequence_;
};

class HttpUploadFilenameGenerator
{
public:
	virtual ~HttpUploadFilenameGenerator(void)
	{
	}

	static HttpUploadFilenameGenerator* parse(const std::string& expression);

	virtual std::string getNextFilename(const HttpRequest& request) = 0;
};

class HttpUploadFilenameGeneratorImpl: public HttpUploadFilenameGenerator
{
public:
	HttpUploadFilenameGeneratorImpl(
			const std::list<FilenamePartGenerator*>& partGenList);

	virtual ~HttpUploadFilenameGeneratorImpl(void);

	virtual std::string getNextFilename(const HttpRequest& request);
private:
	std::list<FilenamePartGenerator*> partGenList_;
};

class HttpSingleFileUploadServlet: public mxhttp::HttpAbstractServlet
{
public:
	HttpSingleFileUploadServlet(void);

	virtual ~HttpSingleFileUploadServlet(void);

	virtual void
	service(mxcore::SharedPtr<HttpVirtualServer> vs,
			mxcore::SharedPtr<HttpRequest> request, HttpResponser* responser);
protected:
	virtual void initImpl(void) throw (mxcore::Exception&);
	bool checkBody(const HttpRequest& request) const;
private:
	std::string rootPath_;
	std::string loggerName_;
	HttpUploadFilenameGenerator* filenameGen_;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPSINGLEFILEUPLOADSERVLET_H__ */
