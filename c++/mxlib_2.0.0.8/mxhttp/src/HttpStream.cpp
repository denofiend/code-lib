/*
 *        Name: HttpStream.cpp
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Assert.h>
#include <mxcore/SystemOps.h>

#include <mxhttp/HttpStream.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

bool HttpStream::isInFile(void) const
{
	return false;
}

bool HttpStream::isInMemory(void) const
{
	return false;
}

HttpStreamInMemory::HttpStreamInMemory(const std::string& content) :
	content_(content), writePos_((int64_t) content.size()), readPos_(0),
			state_(HttpStream::eWriting)
{
}

HttpStreamInMemory::~HttpStreamInMemory(void)
{
}

void HttpStreamInMemory::close(void)
{
	if (eClosed != state_)
	{
		state_ = eClosed;
	}
}

bool HttpStreamInMemory::isClosed(void) const
{
	return eClosed == state_;
}

void HttpStreamInMemory::flip(void)
{
	MX_ASSERT(eClosed != state_);

	if (eWriting == state_)
	{
		state_ = eReading;
		readPos_ = 0;
	}
}

int64_t HttpStreamInMemory::getContentLength(void) const
{
	MX_ASSERT(!isClosed());
	return writePos_;
}

int HttpStreamInMemory::read(void* buf, int len) throw (mxcore::IOException&)
{
	MX_ASSERT(NULL != buf && len > 0);
	MX_ASSERT(eReading == state_);

	if (readPos_ >= (int) content_.size())
	{
		return -1;
	}

	int readLen = (content_.size() - readPos_);
	readLen = len > readLen ? readLen : len;

	::memcpy(buf, content_.c_str() + readPos_, readLen);
	readPos_ += readLen;

	return readLen;
}

int HttpStreamInMemory::write(const void* buf, int len)
		throw (mxcore::IOException&)
{
	MX_ASSERT(NULL != buf && len > 0);
	MX_ASSERT(eWriting == state_);

	content_.append((const char*) buf, len);
	writePos_ += len;
	return len;
}

bool HttpStreamInMemory::isInMemory(void) const
{
	return true;
}

int64_t HttpStreamInMemory::available(void) const
{
	MX_ASSERT(eReading == state_);
	return writePos_ - readPos_;
}

HttpStreamInfile::HttpStreamInfile(FILE* stream) :
	stream_(stream), readPos_(0), state_(eWriting)
{
	MX_ASSERT(NULL != stream_);

	::fseek(stream_, 0, SEEK_END);
	writePos_ = ::ftell(stream_);
}

HttpStreamInfile::~HttpStreamInfile(void)
{
	close();
}

void HttpStreamInfile::close(void)
{
	if (eClosed != state_)
	{
		state_ = eClosed;
		::fclose(stream_);
	}
}

bool HttpStreamInfile::isClosed(void) const
{
	return eClosed == state_;
}

void HttpStreamInfile::flip(void)
{
	MX_ASSERT(eClosed != state_);

	if (eWriting == state_)
	{
		state_ = eReading;
		readPos_ = 0;
		::fseek(stream_, 0, SEEK_SET);
	}
}

int64_t HttpStreamInfile::getContentLength(void) const
{
	return writePos_;
}

int HttpStreamInfile::read(void* buf, int len) throw (mxcore::IOException&)
{
	MX_ASSERT(NULL != buf && len > 0);
	MX_ASSERT(eReading == state_);

	int r = ::fread(buf, 1, len, stream_);

	if (r <= 0)
	{
		if (::feof(stream_))
		{
			return -1;
		}

		THROW3(mxcore::IOException, "Read error", mxos::getLastError());
	}

	readPos_ += r;

	return r;
}

int HttpStreamInfile::write(const void* buf, int len)
		throw (mxcore::IOException&)
{
	MX_ASSERT(NULL != buf && len > 0);
	MX_ASSERT(eWriting == state_);

	int w = ::fwrite(buf, len, 1, stream_);

	if (1 != w)
	{
		THROW3(mxcore::IOException, "Write error", mxos::getLastError());
	}

	writePos_ += len;
	return w;
}

bool HttpStreamInfile::isInFile(void) const
{
	return true;
}

int64_t HttpStreamInfile::available(void) const
{
	MX_ASSERT(eReading == state_);
	return writePos_ - readPos_;
}

} // namespace mxhttp
