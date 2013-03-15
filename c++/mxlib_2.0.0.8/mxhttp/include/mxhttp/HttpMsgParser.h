/*
 *        Name: HttpMsgParser.h
 *
 *  Created on: 2011-2-28
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPMSGPARSER_H__
#define __MXHTTP_HTTPMSGPARSER_H__

#include <mxhttp/HttpMessage.h>
#include <mxhttp/HttpException.h>
#include <mxhttp/HttpParseException.h>
#include <mxhttp/TempFileNameGenerator.h>
#include <mxcore/LoggerFactory.h>

namespace mxhttp
{

class HttpMsgParser
{
	static mxcore::Logger& getLogger(void);
public:
	enum
	{
		eStart = 1, eHeader, eContent, eCompleted
	};

	HttpMsgParser(int64_t maxContentLength, int64_t cachedLimitSize,
			TempFileNameGenerator* tempFilenameGen);
	~HttpMsgParser(void);

	void clear(void);
	int append(const void* buffer, int len) throw (HttpException&);

	bool isIgonContent(void) const;
	void setIgonContent(bool igonContent);

	bool isMoreData(void) const;
	bool isStart(void) const;
	bool isHeader(void) const;
	bool isContent(void) const;
	bool isComplated(void) const;

	HttpMessage* getMessage(void) const;
	HttpRequest* getRequest(void) const;
	HttpResponse* getResponse(void) const;

	HttpMessage* detachMessage(void);
public:
	static void parseParams(HttpRequest* request, const std::string& params);

	static void parseParams(HttpRequest* request, HttpSimpleBody* body)
			throw (HttpParseException&);

	static HttpParam parseParam(const std::string& str);

	static int parseStartLine(HttpMessage*& message, const char* buffer,
			int len) throw (HttpParseException&);

	static int parseHeader(HttpMessage* message, const char* buffer, int len)
			throw (HttpParseException&);

	static int parseHeader(const char* buffer, int len, std::string& name,
			std::string& value);
private:
	int parseHeaders(const char* buffer, int len) throw (HttpParseException&);
	int parseChunked(const char* buffer, int len) throw (HttpParseException&);
	int parseContent(const char* buffer, int len) throw (HttpParseException&);
	int parseMultipart(const char* buffer, int len) throw (HttpParseException&);

	int parseMultipartStart(const char* buffer, int len)
			throw (HttpParseException&);
	int parseMultipartHeaders(const char* buffer, int len)
			throw (HttpParseException&);
	int parseMultipartContent(const char* buffer, int len)
			throw (HttpParseException&);

	int64_t getContentLength(void) const;
	bool checkContentLength(void) const;

	void createBody(void) throw (HttpParseException&);
	bool postProcess(void) throw (HttpParseException&);

	void appendContent(const char* buffer, int len) throw (HttpParseException&);

	static HttpFileItem* createItem(const std::string& disposition);
	static void parseDisposition(const std::string& disposition,
			std::string& name, std::string& filename, bool& isFile)
			throw (HttpParseException&);
private:
	int state_;
	bool igonContent_;

	HttpBody* body_;
	HttpMessage* message_;

	int64_t maxContentLength_;
	int64_t cachedLimitSize_;
	TempFileNameGenerator* tempFilenameGen_;

	mutable int64_t contentLength_;

	int multipartState_;
	HttpFileItem* item_;
	std::string boundary_;
	std::string boundary1_;
	std::string boundaryEnd_;

	int64_t multipartAppendLen_;
};

inline bool HttpMsgParser::isIgonContent(void) const
{
	return igonContent_;
}

inline void HttpMsgParser::setIgonContent(bool igonContent)
{
	igonContent_ = igonContent;
}

inline bool HttpMsgParser::isMoreData(void) const
{
	return eCompleted != state_;
}

inline bool HttpMsgParser::isStart(void) const
{
	return eStart == state_;
}

inline bool HttpMsgParser::isHeader(void) const
{
	return eHeader == state_;
}

inline bool HttpMsgParser::isContent(void) const
{
	return eContent == state_;
}

inline bool HttpMsgParser::isComplated(void) const
{
	return eCompleted == state_;
}

inline HttpMessage* HttpMsgParser::getMessage(void) const
{
	return message_;
}

inline HttpRequest* HttpMsgParser::getRequest(void) const
{
	return (NULL != message_ && message_->isRequest()) ? (HttpRequest*) message_
			: NULL;
}

inline HttpResponse* HttpMsgParser::getResponse(void) const
{
	return (NULL != message_ && message_->isResponse()) ? (HttpResponse*) message_
			: NULL;
}

} // namespace mxhttp

#endif /* __MXHTTP_HTTPMSGPARSER_H__ */
