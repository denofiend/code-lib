/*
 *        Name: HttpUtil.cpp
 *
 *  Created on: 2011-2-24
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <fstream>
#include <mxcore/SocketOps.h>
#include <mxcore/UrlCodec.h>
#include <mxhttp/HttpUtil.h>
#include <mxcore/SystemOps.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpResponse* HttpUtil::makeResponse(const HttpRequest& request,
		const std::pair<int, std::string>& status, const std::string& body,
		const std::string& contentType)
{
	return makeResponse(request, status.first, status.second, body, contentType);
}

HttpResponse* HttpUtil::makeResponse(const HttpRequest& request,
		int statusCode, const std::string& reasonPhrase,
		const std::string& body, const std::string& contentType)
{
	HttpResponse* response = NULL;

	if (body.empty())
	{
		response = makeResponse(request, statusCode, reasonPhrase, NULL);
		if (!contentType.empty())
		{
			response->getHeaders().add(HttpConstants::HEADER_CONTENT_TYPE,
					contentType);
		}
	}
	else
	{
		HttpSimpleBody* simpleBody = HttpSimpleBody::makeInMemory(body);
		simpleBody->flip();
		simpleBody->setContentType(contentType);
		response = makeResponse(request, statusCode, reasonPhrase, simpleBody);
	}

	return response;
}

HttpResponse* HttpUtil::makeResponse(const HttpRequest& request,
		int statusCode, const std::string& reasonPhrase, HttpBody* body)
{
	HttpResponse* response = new HttpResponse;
	makeResponse(request, *response, statusCode, reasonPhrase, body);
	return response;
}

HttpResponse* HttpUtil::makeResponse(const HttpRequest& request,
		const std::pair<int, std::string>& status, HttpBody* body)
{
	return makeResponse(request, status.first, status.second, body);
}

void HttpUtil::makeResponse(const HttpRequest& request, HttpResponse& response,
		int statusCode, const std::string& reasonPhrase, HttpBody* body)
{
	MX_ASSERT(NULL == body || body->isSimpleBody());

	response.setConnectionId(request.getConnectionId());
	response.setStatusCode(statusCode);
	response.setReasonPhrase(reasonPhrase);
	response.setBody(body);

	response.getHeaders().remove(HttpConstants::HEADER_HOST);
	response.getHeaders().add(HttpConstants::HEADER_HOST,
			request.getHeaders().getValue(HttpConstants::HEADER_HOST));
}

void HttpUtil::makeResponse(const HttpRequest& request, HttpResponse& response,
		const std::pair<int, std::string>& status, HttpBody* body)
{
	return makeResponse(request, response, status.first, status.second, body);
}

HttpResponse* HttpUtil::makeFileResponse(const HttpRequest& request,
		const std::string& filepath, bool isTemp)
{
	HttpSimpleBody* body = NULL;

	if (!mxos::isFileExists(filepath.c_str()))
	{
		return makeResponse(request, HttpConstants::STATUS_NOT_FOUND, body);
	}

	try
	{
		body = HttpSimpleBody::makeInFile(filepath, false);
		body->flip();

		HttpResponse* response = makeResponse(request,
				HttpConstants::STATUS_OK, body);
		response->getHeaders().add(HttpConstants::HEADER_CONTENT_TYPE,
				HttpUtil::getFileContentType(filepath));
		return response;
	} catch (mxcore::IOException& e)
	{
		if (NULL != body)
		{
			delete body;
		}

		return makeResponse(request, HttpConstants::STATUS_SERVER_ERROR, NULL);
	}
}

void HttpUtil::encodeRequestHeaders(HttpRequest& request, std::string& str)
{
	bool encodedParams = HttpUtil::encodeStartLine(request, str);
	str += HttpConstants::CRLF;

	request.getHeaders().remove(HttpConstants::HEADER_CONTENT_LENGTH);

	std::string params;
	HttpBody* body = request.getBody();

	if (NULL != body)
	{
		if (body->isSimpleBody())
		{
			HttpSimpleBody* simpleBody = (HttpSimpleBody*) body;

			if (!simpleBody->getContentType().empty())
			{
				request.getHeaders().remove(HttpConstants::HEADER_CONTENT_TYPE);
				request.getHeaders().add(HttpConstants::HEADER_CONTENT_TYPE,
						simpleBody->getContentType());
			}
			request.getHeaders().add(HttpConstants::HEADER_CONTENT_LENGTH,
					mxcore::Long(simpleBody->getContentLength()).toString());
		}
		else
		{
			HttpMultipartBody* multipartBody = (HttpMultipartBody*) body;

			request.getHeaders().remove(HttpConstants::HEADER_CONTENT_TYPE);
			std::string contentType("multipart/form-data; boundary=");
			contentType += ((HttpMultipartBody*) body)->getBoundary();
			request.getHeaders().add(HttpConstants::HEADER_CONTENT_TYPE,
					contentType);

			request.getHeaders().add(
					HttpConstants::HEADER_CONTENT_LENGTH,
					mxcore::Long(HttpUtil::buildContentLength(multipartBody)).toString());
		}
	}
	else
	{
		if (!encodedParams && (request.getParams().getCount() > 0))
		{
			encodeParams(request.getParams(), params);

			request.getHeaders().add(HttpConstants::HEADER_CONTENT_LENGTH,
					mxcore::Long((int64_t) params.size()).toString());
		}
	}

	encodeHeaders(request.getHeaders(), str);
	str += HttpConstants::CRLF;

	if (!params.empty())
	{
		str += params;
	}
}

void HttpUtil::encodeResponseHeaders(HttpResponse& response, std::string& str)
{
	HttpUtil::encodeStartLine(response, str);
	str += HttpConstants::CRLF;

	response.getHeaders().remove(HttpConstants::HEADER_CONTENT_LENGTH);

	HttpBody* body = response.getBody();

	if (NULL != body)
	{
		MX_ASSERT(body->isSimpleBody());
		HttpSimpleBody* simpleBody = (HttpSimpleBody*) body;
		response.getHeaders().add(HttpConstants::HEADER_CONTENT_LENGTH,
				mxcore::Long(simpleBody->getContentLength()).toString());

		if (!simpleBody->getContentType().empty())
		{
			response.getHeaders().remove(HttpConstants::HEADER_CONTENT_TYPE);
			response.getHeaders().add(HttpConstants::HEADER_CONTENT_TYPE,
					simpleBody->getContentType());
		}
	}
	else
	{
		response.getHeaders().add(HttpConstants::HEADER_CONTENT_LENGTH,
				mxcore::Long(0L).toString());
	}

	encodeHeaders(response.getHeaders(), str);
	str += HttpConstants::CRLF;
}

void HttpUtil::encodeParams(const HttpParamList& params, std::string& str)
{
	int paramCount = params.getCount();

	for (int i = 0; i < paramCount; i++)
	{
		const HttpParam& param = params.get(i);

		str += mxcore::UrlEncoder::encode(param.first, "UTF-8");
		str += "=";
		str += mxcore::UrlEncoder::encode(param.second, "UTF-8");

		if (i != (paramCount - 1))
		{
			str += "&";
		}
	}
}

void HttpUtil::encodeHeaders(const HttpHeaderList& headers, std::string& str)
{
	int headerCount = headers.getCount();

	for (int i = 0; i < headerCount; i++)
	{
		const HttpHeader& header = headers.get(i);
		str += header.first;
		str += ": ";
		str += header.second;
		str += HttpConstants::CRLF;
	}
}

bool HttpUtil::encodeStartLine(HttpRequest& request, std::string& str)
{
	bool encodedParams = false;

	str += request.getMethod();
	str += " ";
	str += request.getUri();

	if (!request.isPost() || request.getBody() != NULL)
	{
		if (request.getParams().getCount() > 0)
		{
			str += "?";
			encodeParams(request.getParams(), str);
		}
		encodedParams = true;
	}

	str += " ";
	str += request.getVersion();

	return encodedParams;
}

void HttpUtil::encodeStartLine(HttpResponse& response, std::string& str)
{
	str += response.getVersion();
	str += " ";
	str += mxcore::Integer(response.getStatusCode()).toString();
	str += " ";
	str += response.getReasonPhrase();
}

int64_t HttpUtil::buildContentLength(const HttpMultipartBody* body)
{
	int64_t contentLength = 0;

	const static std::string FORM_DATA("form-data");

	for (int i = 0; i < body->getItemCount(); i++)
	{
		HttpFileItem* item = body->getItem(i);
		contentLength += 2 /* for --*/+ body->getBoundary().size() + 2 /* for \r\n*/;
		contentLength
				+= (int64_t) HttpConstants::HEADER_CONTENT_DISPOSITION.size()
						+ 2 /* for : */;
		contentLength += (int64_t) FORM_DATA.size() + 2 /* for ; */;
		contentLength += 6 /* for name="*/;
		contentLength += (int64_t) item->getName().size() + 1 /* for "*/;

		if (!item->isFormField())
		{
			contentLength += 2 /*for ; */+ 10 /* for filename="*/;
			contentLength
					+= (int64_t) ((HttpUploadFileItem*) item)->getDisplayFilepath().size();
			contentLength += 1 /*for "*/;
		}
		contentLength += 2 /*for \r\n*/;

		for (int j = 0; j < item->getHeaders().getCount(); j++)
		{
			contentLength += (int64_t) item->getHeaders().get(j).first.size();
			contentLength += 2 /*for : */;
			contentLength += (int64_t) item->getHeaders().get(j).second.size();
			contentLength += 2 /*for \r\n*/;
		}

		contentLength += 2 /*for \r\n*/;
		contentLength += item->getContentLength();
		contentLength += 2 /* for \r\n*/;
	}

	contentLength += 2 /*for --*/+ (int64_t) body->getBoundary().size() + 2
	/*for --*/+ 2 /*for \r\n*/;

	return contentLength;
}

const std::string& HttpUtil::getFileContentType(const std::string& filepath)
{
	if (mxcore::StringUtil::endWith(filepath, ".htm")
			|| mxcore::StringUtil::endWith(filepath, ".html"))
	{
		static std::string str("text/html");
		return str;
	}
	else if (mxcore::StringUtil::endWith(filepath, ".jpg"))
	{
		static std::string str("image/jpeg");
		return str;
	}
	else if (mxcore::StringUtil::endWith(filepath, ".bmp"))
	{
		static std::string str("image/bmp");
		return str;
	}
	else if (mxcore::StringUtil::endWith(filepath, ".ico"))
	{
		static std::string str("image/x-icon");
		return str;
	}
	else if (mxcore::StringUtil::endWith(filepath, ".txt"))
	{
		static std::string str("text/plain");
		return str;
	}
	else
	{
		static std::string str("application/octet-stream");
		return str;
	}
}

template<class T>
static void writeToFileT(T* reader, const std::string& filename)
		throw (mxcore::IOException&)
{
	char buffer[4096];
	std::ofstream out;
	out.open(filename.c_str());

	if (out.fail())
	{
		THROW3(mxcore::IOException, std::string("Open file: [") + filename + "]", mxos::getLastError());
	}

	try
	{

		int n = 0;

		do
		{
			int n = reader->read(buffer, 4096);

			if (n > 0)
			{
				out.write(buffer, n);
			}
		} while (n > 0);
	} catch (mxcore::IOException& e)
	{
		::remove(filename.c_str());
		throw e;
	}
}

void HttpUtil::writeToFile(HttpUploadFileItem* item,
		const std::string& filename) throw (mxcore::IOException&)
{
	writeToFileT<HttpUploadFileItem> (item, filename);
}

void HttpUtil::writeToFile(HttpSimpleBody* body, const std::string& filename)
		throw (mxcore::IOException&)
{
	writeToFileT<HttpSimpleBody> (body, filename);
}

} // namespace mxhttp
