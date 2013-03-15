/*
 *        Name: HttpSimpleBody.cpp
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/SystemOps.h>
#include <mxhttp/HttpSimpleBody.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpSimpleBody::HttpSimpleBody(int type, HttpStream* stream) :
	type_(type), stream_(stream)
{
}

HttpSimpleBody::~HttpSimpleBody(void)
{
	if (NULL != stream_)
	{
		delete stream_;
		stream_ = NULL;
	}
}

bool HttpSimpleBody::isSimpleBody(void) const
{
	return true;
}

void HttpSimpleBody::flip(void)
{
	stream_->flip();
}

int64_t HttpSimpleBody::getContentLength(void) const
{
	return stream_->getContentLength();
}

int64_t HttpSimpleBody::available(void) const
{
	return stream_->available();
}

int HttpSimpleBody::read(void* buf, int len) throw (mxcore::IOException&)
{
	return stream_->read(buf, len);
}

int HttpSimpleBody::write(const void* buf, int len)
		throw (mxcore::IOException&)
{
	return stream_->write(buf, len);
}

void HttpSimpleBody::closeImpl(void)
{
	stream_->close();
}

HttpSimpleBody* HttpSimpleBody::makeInFile(const std::string& filepath,
		bool isTemp) throw (mxcore::IOException&)
{
	FILE* stream = NULL;

	if (isTemp)
	{
		stream = ::fopen(filepath.c_str(), "w+");
	}
	else
	{
		stream = ::fopen(filepath.c_str(), "r");
	}

	if (NULL == stream)
	{
		THROW3(mxcore::IOException, std::string("Open file: [" + filepath +"]"), mxos::getLastError());
	}

	return new HttpSimpleBodyInFile(stream, filepath, isTemp);
}

HttpSimpleBody* HttpSimpleBody::makeInFile(HttpSimpleBody* srcBody,
		const std::string& filepath, bool isTemp) throw (mxcore::IOException&)
{
	HttpSimpleBody* infile = NULL;

	try
	{
		infile = makeInFile(filepath, isTemp);

		if (NULL != infile)
		{
			char buffer[1024];

			srcBody->flip();

			int r = srcBody->read(buffer, 1024);

			while (r > 0)
			{
				infile->write(buffer, r);
				r = srcBody->read(buffer, 1024);
			}
		}

		return infile;
	} catch (mxcore::IOException& e)
	{
		delete infile;
		throw e;
	}
}

HttpSimpleBody* HttpSimpleBody::makeInMemory(const std::string& content)
{
	return new HttpSimpleBodyInMemory(content);
}

HttpSimpleBodyInMemory::HttpSimpleBodyInMemory(const std::string& content) :
	HttpSimpleBody(HttpSimpleBody::eInMemory, new HttpStreamInMemory(content))
{
}

HttpSimpleBodyInMemory::~HttpSimpleBodyInMemory(void)
{
	close();
}

HttpSimpleBodyInFile::HttpSimpleBodyInFile(FILE* stream,
		const std::string& filepath, bool removeFileOnClose) :
	HttpSimpleBody(HttpSimpleBody::eInFile, new HttpStreamInfile(stream)),
			filepath_(filepath), removeFileOnClose_(removeFileOnClose)
{
}

HttpSimpleBodyInFile::~HttpSimpleBodyInFile(void)
{
	close();
}

void HttpSimpleBodyInFile::closeImpl(void)
{
	stream_->close();

	if (removeFileOnClose_)
	{
		removeFileOnClose_ = false;
		::remove(filepath_.c_str());
	}
}

} // namespace mxhttp
