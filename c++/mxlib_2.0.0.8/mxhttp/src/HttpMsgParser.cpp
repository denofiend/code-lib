/*
 *        Name: HttpMsgParser.cpp
 *
 *  Created on: 2011-2-28
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <mxcore/Assert.h>
#include <mxcore/UrlCodec.h>
#include <mxcore/StringUtil.h>

#include <mxhttp/HttpMsgParser.h>
#include <mxhttp/HttpParseException.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

mxcore::Logger& HttpMsgParser::getLogger(void)
{
	return mxcore::LoggerFactory::getInstance().getLogger("mxhttp");
}

HttpMsgParser::HttpMsgParser(int64_t maxContentLength, int64_t cachedLimitSize,
		TempFileNameGenerator* tempFilenameGen) :
	state_(eStart), igonContent_(false), body_(NULL), message_(NULL),
			maxContentLength_(maxContentLength),
			cachedLimitSize_(cachedLimitSize),
			tempFilenameGen_(tempFilenameGen), contentLength_(-1L),
			multipartState_(eStart), item_(NULL), multipartAppendLen_(0)
{
}

HttpMsgParser::~HttpMsgParser(void)
{
	clear();
}

void HttpMsgParser::clear(void)
{
	if (NULL != message_)
	{
		delete message_;
		message_ = NULL;
	}

	if (NULL != body_)
	{
		delete body_;
		body_ = NULL;
	}

	if (NULL != item_)
	{
		delete item_;
		item_ = NULL;
	}

	state_ = eStart;
	multipartState_ = eStart;
	multipartAppendLen_ = 0;
	contentLength_ = -1L;
}

int HttpMsgParser::append(const void* buffer, int len) throw (HttpException&)
{
	int parsedLen = 0;

	MX_ASSERT(NULL != buffer && len > 0);
	MX_ASSERT(isMoreData());

	const char* buf = (const char*) buffer;

	if (isStart())
	{
		parsedLen = parseStartLine(message_, buf, len);
		if (0 == parsedLen)
		{
			return parsedLen;
		}

		state_ = eHeader;
	}

	MX_ASSERT(NULL != message_);

	// parse headers
	if (isHeader())
	{
		parsedLen += parseHeaders(buf + parsedLen, len - parsedLen);

		if (isContent())
		{
			int64_t contentLen = getContentLength();

			if (contentLen <= 0 && !message_->isChunked())
			{
				state_ = eCompleted;
				return parsedLen;
			}

			if (!checkContentLength())
			{
				THROW_HTTP_EXCEPTION3(HttpParseException, std::string(
								"ContentLength header specific length too large: [")
						+ message_->getHeaders().getValue(
								HttpConstants::HEADER_CONTENT_LENGTH),
						HttpConstants::STATUS_ENTITY_TO0_LARGE);
			}

			if (message_->isRequest()
					&& ((HttpRequest*) message_)->isMultiPart())
			{
				boundary_ = ((HttpRequest*) message_)->getBoundary();

				if (boundary_.empty())
				{
					THROW_HTTP_EXCEPTION3(HttpParseException, "Empty boundary",
							HttpConstants::STATUS_BAD_REQUEST);
				}

				boundary_.insert(0, "--");
				boundary1_ = HttpConstants::CRLF;
				boundary1_ += boundary_;

				boundaryEnd_ = boundary_;
				boundaryEnd_ += "--";
			}
		}
	}

	if (message_->isResponse() && isContent() && isIgonContent())
	{
		state_ = eCompleted;
		return parsedLen;
	}

	if (isContent() && (len - parsedLen > 0))
	{
		createBody();

		if (message_->isChunked())
		{
			parsedLen += parseChunked(buf + parsedLen, len - parsedLen);
		}
		else if (message_->isRequest()
				&& ((HttpRequest*) message_)->isMultiPart())
		{
			parsedLen += parseMultipart(buf + parsedLen, len - parsedLen);

			if (eCompleted == multipartState_)
			{
				state_ = eCompleted;
			}
		}
		else
		{
			parsedLen += parseContent(buf + parsedLen, len - parsedLen);
		}
	}

	if (eCompleted == state_)
	{
		if (NULL != body_ && ((HttpSimpleBody*) body_)->isSimpleBody())
		{
			((HttpSimpleBody*) body_)->flip();

			if (postProcess())
			{
				delete body_;
				body_ = NULL;
			}
		}

		message_->setBody(body_);
		body_ = NULL;
	}

	return parsedLen;
}

HttpMessage* HttpMsgParser::detachMessage(void)
{
	HttpMessage* ret = message_;
	message_ = NULL;
	clear();
	return ret;
}

void HttpMsgParser::parseParams(HttpRequest* request, const std::string& params)
{
	size_t pos = 0;
	std::string paramStr;

	while (mxcore::StringUtil::nextSeparator(params, pos, "&", paramStr))
	{
		HttpParam param = parseParam(paramStr);

		if (!param.first.empty())
		{
			request->getParams().add(param);
		}
	}
}

void HttpMsgParser::parseParams(HttpRequest* request, HttpSimpleBody* body)
		throw (HttpParseException&)
{
	std::string params;
	char buffer[1024 + 1] =
	{ 0 };

	try
	{
		int r = body->read(buffer, 1024);

		for (; r > 0;)
		{
			params.append(buffer, r);
			r = body->read(buffer, 1024);
		}

		parseParams(request, params);
	} catch (mxcore::IOException& e)
	{
		THROW_HTTP_EXCEPTION4(HttpParseException, e.getMessage(), e.getErrorCode(),
				HttpConstants::STATUS_SERVER_ERROR);
	}
}

HttpParam HttpMsgParser::parseParam(const std::string& str)
{
	size_t pos = str.find('=');

	std::string value;
	std::string name = str.substr(0, pos);
	mxcore::StringUtil::trim(name);

	if (pos != std::string::npos && (pos + 1) < str.length())
	{
		value = str.substr(pos + 1);
		value = mxcore::UrlDecoder::decode(mxcore::StringUtil::trim(value));
	}

	return std::make_pair(name, value);
}

int HttpMsgParser::parseStartLine(HttpMessage*& message, const char* buffer,
		int len) throw (HttpParseException&)
{
	int lineLen = 0;
	size_t pos = 0;

	std::string firstPart;
	std::string secondPart;
	std::string threePart;

	const char* lineEnd = (const char*) ::memchr(buffer, '\n', len);

	if (NULL == lineEnd)
	{
		return lineLen;
	}

	lineLen = lineEnd - buffer + 1;

	std::string line(buffer, lineLen);
	mxcore::StringUtil::trim(line);

	if (!mxcore::StringUtil::nextSeparator(line, pos, " ", firstPart))
	{
		THROW_HTTP_EXCEPTION3(HttpParseException, std::string(
						"Not found the first part of start line: [") + line + "]",
				HttpConstants ::STATUS_BAD_REQUEST);
	}

	if (!mxcore::StringUtil::nextSeparator(line, pos, " ", secondPart))
	{
		THROW_HTTP_EXCEPTION3(HttpParseException, std::string(
						"Not found the second part of start line: [") + line + "]",
				HttpConstants ::STATUS_BAD_REQUEST);
	}

	if (std::string::npos != pos)
	{
		threePart = line.substr(pos);
	}

	if (mxcore::StringUtil::compareNoCaseN(firstPart, "HTTP", 4) == 0)
	{
		// This is a response
		mxcore::Integer statusCode;
		try
		{
			statusCode = mxcore::Integer::fromString(secondPart);
		} catch (mxcore::NumberFormatException& e)
		{
			THROW_HTTP_EXCEPTION3(HttpParseException, std::string(
							"Invalid status code of start line: [") + line + "]",
					HttpConstants ::STATUS_BAD_REQUEST);
		}

		HttpResponse* response = new HttpResponse;
		response->setVersion(firstPart);
		response->setStatusCode(statusCode);
		response->setReasonPhrase(threePart);

		message = response;
	}
	else
	{
		size_t paramPos = secondPart.find('?');

		if (paramPos == 0)
		{
			THROW_HTTP_EXCEPTION3(HttpParseException, std::string(
							"Invalid request uri of start line: [") + line + "]",
					HttpConstants ::STATUS_BAD_REQUEST);
		}

		HttpRequest* request = new HttpRequest;
		request->setMethod(firstPart);

		if (std::string::npos != paramPos)
		{
			std::string params = secondPart.substr(paramPos + 1);
			parseParams(request, params);
			request ->setUri(secondPart.substr(0, paramPos));
		}
		else
		{
			request->setUri(secondPart);
		}

		request->setVersion(threePart);

		message = request;
	}

	return lineLen;
}

int HttpMsgParser::parseHeader(HttpMessage* message, const char* buffer,
		int len) throw (HttpParseException&)
{
	int lineLen = 0;
	const char* lineEnd = (const char*) ::memchr(buffer, '\n', len);

	if (NULL != lineEnd)
	{
		lineLen = lineEnd - buffer + 1;

		std::string line(buffer, lineLen);
		mxcore::StringUtil::trim(line);

		size_t pos = line.find(':');

		if (std::string::npos == pos && 0 == pos)
		{
			THROW_HTTP_EXCEPTION3(HttpParseException, "No header name found", HttpConstants::STATUS_BAD_REQUEST);
		}
		else
		{
			std::string value;
			std::string name = line.substr(0, pos);

			if (pos + 1 < line.size())
			{
				value = line.substr(pos + 1);
			}

			message->getHeaders().add(mxcore::StringUtil::trimRight(name),
					mxcore::StringUtil::trimLeft(value));
		}
	}

	return lineLen;
}

int HttpMsgParser::parseHeader(const char* buffer, int len, std::string& name,
		std::string& value)
{
	int lineLen = 0;

	name.clear();
	value.clear();

	const char* lineEnd = (const char*) ::memchr(buffer, '\n', len);

	if (NULL != lineEnd)
	{
		lineLen = lineEnd - buffer + 1;

		std::string line(buffer, lineLen);
		mxcore::StringUtil::trim(line);

		size_t pos = line.find(':');

		if (std::string::npos != pos && pos > 0)
		{
			name = line.substr(0, pos);
			mxcore::StringUtil::trimRight(name);

			if (pos + 1 < line.length())
			{
				value = line.substr(pos + 1);
				mxcore::StringUtil::trimLeft(value);
			}
		}
	}

	return lineLen;
}

int HttpMsgParser::parseHeaders(const char* buffer, int len)
		throw (HttpParseException&)
{
	int parsedLen = 0;

	for (int headerLen = 1; eHeader == state_ && headerLen > 0 && parsedLen
			< len; parsedLen += headerLen)
	{
		headerLen = parseHeader(message_, buffer + parsedLen, len - parsedLen);

		char ch = *(buffer + parsedLen);

		if ('\r' == ch || '\n' == ch)
		{
			state_ = eContent;
		}
	}

	return parsedLen;
}

static int getLine(const char* buffer, int len, char * line, int size)
{
	int lineLen = 0;

	const char* lineEnd = (const char*) ::memchr(buffer, '\n', len);

	if (NULL != lineEnd)
	{
		lineLen = lineEnd - buffer + 1;

		int realLen = size - 1;
		realLen = realLen > lineLen ? lineLen : realLen;

		::memcpy(line, buffer, realLen);
		line[realLen] = '\0';
		::strtok(line, "\r\n");
	}

	return lineLen;
}

int HttpMsgParser::parseChunked(const char* buffer, int len)
		throw (HttpParseException&)
{
	int parsedLen = 0;
	bool hasChunk = true;

	for (; hasChunk && isContent();)
	{
		hasChunk = 0;

		char chunkSize[32] =
		{ 0 };

		int lineLen = getLine(((char*) buffer) + parsedLen, len - parsedLen,
				chunkSize, sizeof(chunkSize));

		if (lineLen <= 0)
		{
			break;
		}

		int contentLen = ::strtol(chunkSize, NULL, 16);

		if (contentLen > 0 && (len - parsedLen) > (contentLen + lineLen))
		{
			int emptyLen = getLine(buffer + parsedLen + lineLen + contentLen,
					len - parsedLen - lineLen - contentLen, chunkSize,
					sizeof(chunkSize));

			if (emptyLen > 0)
			{
				parsedLen += lineLen;

				appendContent(buffer + parsedLen, contentLen);

				parsedLen += (contentLen + emptyLen);

				hasChunk = 1;
			}
		}

		if (0 == contentLen && lineLen > 0)
		{
			parsedLen += lineLen;
			state_ = eCompleted;
		}
	}

	return parsedLen;
}

int HttpMsgParser::parseContent(const char* buffer, int len)
		throw (HttpParseException&)
{
	MX_ASSERT(NULL != buffer && len > 0);
	MX_ASSERT(NULL != body_ && body_->isSimpleBody());

	int64_t contentLen = getContentLength();

	len = len > contentLen ? contentLen : len;

	appendContent(buffer, len);

	if (((HttpSimpleBody*) body_)->getContentLength() >= contentLen)
	{
		state_ = eCompleted;
	}

	return len;
}

int HttpMsgParser::parseMultipart(const char* buffer, int len)
		throw (HttpParseException&)
{
	int tmpLen = 0;
	int parsedLen = 0;

	do
	{
		if (eStart == multipartState_)
		{
			tmpLen = parseMultipartStart(buffer + parsedLen, len - parsedLen);
			parsedLen += tmpLen;
			multipartAppendLen_ += tmpLen;
		}

		if (eHeader == multipartState_ && (len - parsedLen > 0))
		{
			tmpLen = parseMultipartHeaders(buffer + parsedLen, len - parsedLen);
			parsedLen += tmpLen;
			multipartAppendLen_ += tmpLen;
		}

		if (eContent == multipartState_ && (len - parsedLen > 0))
		{
			tmpLen = parseMultipartContent(buffer + parsedLen, len - parsedLen);
			parsedLen += tmpLen;
			multipartAppendLen_ += tmpLen;
		}
	} while (eStart == multipartState_ && (len - parsedLen) > 0 && tmpLen > 0);

	return parsedLen;
}

int HttpMsgParser::parseMultipartStart(const char* buffer, int len)
		throw (HttpParseException&)
{
	int parsedLen = 0;

	while (eStart == multipartState_ && (len - parsedLen) > 0)
	{
		std::string line;

		int lineLen = mxcore::StringUtil::getLine(buffer + parsedLen,
				len - parsedLen, line);

		if (lineLen <= 0)
		{
			return parsedLen;
		}

		if (line.empty())
		{
			parsedLen += lineLen;
			continue;
		}

		if (line == boundary_)
		{
			multipartState_ = eHeader;
			parsedLen += lineLen;
		}
		else if (line == boundaryEnd_)
		{
			multipartState_ = eCompleted;
			parsedLen += lineLen;
		}
		else
		{
			THROW_HTTP_EXCEPTION3(HttpParseException, "The first line is not boundary", HttpConstants::STATUS_BAD_REQUEST);
		}
	}

	return parsedLen;
}

int HttpMsgParser::parseMultipartHeaders(const char* buffer, int len)
		throw (HttpParseException&)
{
	int parsedLen = 0;

	if (NULL == item_)
	{
		std::string name;
		std::string value;

		int headerLen = parseHeader(buffer + parsedLen, len - parsedLen, name,
				value);

		if (headerLen <= 0)
		{
			return parsedLen;
		}

		parsedLen += headerLen;

		if (!mxcore::StringUtil::equalNoCase(name,
				HttpConstants::HEADER_CONTENT_DISPOSITION))
		{
			THROW_HTTP_EXCEPTION3(HttpParseException, "The first header name is not [%s]\n",
					HttpConstants::STATUS_BAD_REQUEST);
		}

		item_ = createItem(value);
		item_->getHeaders().add(name, value);
	}

	for (int headerLen = 1; eHeader == multipartState_ && headerLen > 0
			&& parsedLen < len; parsedLen += headerLen)
	{
		std::string name;
		std::string value;

		headerLen = parseHeader(buffer + parsedLen, len - parsedLen, name,
				value);

		if (!name.empty())
		{
			item_->getHeaders().add(name, value);
		}

		char ch = *(buffer + parsedLen);

		if ('\r' == ch || '\n' == ch)
		{
			multipartState_ = eContent;
		}
	}

	return parsedLen;
}

int HttpMsgParser::parseMultipartContent(const char* buffer, int len)
		throw (HttpParseException&)
{
	int parsedLen = 0;

	if (len < (int) boundary1_.length())
	{
		return parsedLen;
	}

	const char* pos = (const char*) mxcore::StringUtil::memStr(buffer, len,
			boundary1_.c_str(), (int) boundary1_.length());

	if (NULL == pos)
	{
		parsedLen = len - (int) boundary1_.length();

		if (parsedLen > 0)
		{
			item_->write(buffer, parsedLen);
		}
	}
	else
	{
		parsedLen = pos - buffer;

		std::string s(pos, boundary1_.length());

		if (parsedLen > 0)
		{
			item_->write(buffer, parsedLen);
		}

		multipartState_ = eStart;
		item_->flip();

		((HttpMultipartBody*) body_)->addItem(item_);
		item_ = NULL;
		parsedLen += 2;
	}

	return parsedLen;
}

int64_t HttpMsgParser::getContentLength(void) const
{
	MX_ASSERT(NULL != message_);

	if (-1L == contentLength_)
	{
		std::string strContentLength = message_->getHeaders().getValue(
				HttpConstants::HEADER_CONTENT_LENGTH);

		if (strContentLength.empty())
		{
			contentLength_ = 0;
		}

		contentLength_ = ::atoll(strContentLength.c_str());
	}

	return contentLength_;
}

bool HttpMsgParser::checkContentLength(void) const
{
	return (maxContentLength_ <= 0 || maxContentLength_ >= getContentLength());
}

void HttpMsgParser::createBody(void) throw (HttpParseException&)
{
	if (NULL != body_)
	{
		return;
	}

	if (message_->isChunked())
	{
		body_ = HttpSimpleBody::makeInMemory("");
		return;
	}

	try
	{
		if (getContentLength() > 0)
		{
			if (message_->isRequest()
					&& ((HttpRequest*) message_)->isMultiPart())
			{
				body_ = HttpMultipartBody::makeMultipartBody(
						((HttpRequest*) message_)->getBoundary());
			}
			else
			{
				if (getContentLength() > cachedLimitSize_)
				{
					body_ = HttpSimpleBody::makeInFile(
							tempFilenameGen_->getNextFileName(), true);
				}
				else
				{
					body_ = HttpSimpleBody::makeInMemory("");
				}
			}
		}
	} catch (mxcore::IOException& e)
	{
		THROW_HTTP_EXCEPTION4(HttpParseException, e.getMessage()
				, e.getErrorCode(), HttpConstants::STATUS_SERVER_ERROR);
	}
}

bool HttpMsgParser::postProcess(void) throw (HttpParseException&)
{
	if (message_->isRequest() && ((HttpRequest*) message_)->isFormUrlEncoded())
	{
		parseParams((HttpRequest*) message_, (HttpSimpleBody*) body_);
		return true;
	}

	return false;
}

void HttpMsgParser::appendContent(const char* buffer, int len)
		throw (HttpParseException&)
{
	MX_ASSERT(NULL != body_ && body_->isSimpleBody());

	try
	{
		HttpSimpleBody* body = (HttpSimpleBody*) body_;

		if ((body->getContentLength() + len) > cachedLimitSize_
				&& body->isInMemory())
		{
			HttpSimpleBody* infile = HttpSimpleBody::makeInFile(body,
					tempFilenameGen_->getNextFileName(), true);

			delete body;
			body_ = infile;
			body = infile;
		}

		body->write(buffer, len);
	} catch (mxcore::IOException& e)
	{
		THROW_HTTP_EXCEPTION4(HttpParseException, e.getMessage()
				, e.getErrorCode(), HttpConstants::STATUS_SERVER_ERROR);
	}
}

HttpFileItem* HttpMsgParser::createItem(const std::string& disposition)
{
	std::string name;
	std::string filename;
	bool isFile = false;

	parseDisposition(disposition, name, filename, isFile);

	if (!isFile)
	{
		return HttpFileItem::makeFormFieldItem(name);
	}
	else
	{
		return HttpFileItem::makeInMemoryUploadFileItem(name, filename);
	}
}

void HttpMsgParser::parseDisposition(const std::string& disposition,
		std::string& name, std::string& filename, bool& isFile)
		throw (HttpParseException&)
{
	std::string paramStr;
	size_t nextPos = 0;

	filename.clear();
	isFile = false;

	for (; nextPos < disposition.length();)
	{
		std::string paramName;
		std::string paramValue;
		size_t prevPos = nextPos;
		nextPos = disposition.find(';', nextPos);

		if (nextPos != std::string::npos)
		{
			paramStr = disposition.substr(prevPos, nextPos - prevPos);
		}
		else
		{
			paramStr = disposition.substr(prevPos);
		}

		HttpParam param = parseParam(paramStr);

		paramName = param.first;
		paramValue = param.second;

		if (!paramName.empty() && mxcore::StringUtil::equalNoCase("name",
				paramName))
		{
			mxcore::StringUtil::removeQuote(paramValue);

			if (paramValue.empty())
			{
				THROW_HTTP_EXCEPTION3(HttpParseException, std::string("Parse disposition: [") + disposition + "], the name value is empty"
						, HttpConstants::STATUS_BAD_REQUEST);
			}
			name = paramValue;
		}
		else if (!paramName.empty() && mxcore::StringUtil::equalNoCase(
				"filename", paramName))
		{
			isFile = true;
			filename = mxcore::StringUtil::removeQuote(paramValue);
		}

		if (nextPos == std::string::npos)
		{
			break;
		}

		nextPos += 1;
	}

	if (name.empty())
	{
		THROW_HTTP_EXCEPTION3(HttpParseException, std::string("Parse disposition: [") + disposition + "], not found the name param"
				, HttpConstants::STATUS_BAD_REQUEST);
	}
}

} // namespace mxhttp
