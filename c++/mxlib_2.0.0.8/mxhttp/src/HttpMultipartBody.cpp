/*
 *        Name: HttpMultipartBody.cpp
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Assert.h>
#include <mxcore/SystemOps.h>

#include <mxhttp/HttpMultipartBody.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpFileItem::HttpFileItem(int type, const std::string& name,
		HttpStream* stream) :
	type_(type), name_(name), stream_(stream)
{
	MX_ASSERT(NULL != stream_);
}

HttpFileItem::~HttpFileItem(void)
{
	if (NULL != stream_)
	{
		delete stream_;
		stream_ = NULL;
	}
}

void HttpFileItem::closeImpl(void)
{
}

HttpFormFieldItem* HttpFileItem::makeFormFieldItem(const std::string& name)
{
	return new HttpFormFieldItem(name);
}

HttpUploadFileInMemory* HttpFileItem::makeInMemoryUploadFileItem(
		const std::string& name, const std::string& displayFilepath)
{
	return new HttpUploadFileInMemory(name, displayFilepath);
}

HttpUploadFileInFile* HttpFileItem::makeInFileUploadFileItem(
		const std::string& name, const std::string& displayFilepath,
		const std::string& srcFilepath, bool isTemp)
		throw (mxcore::IOException&)
{
	FILE* stream = ::fopen(srcFilepath.c_str(), "rw");

	if (NULL == stream)
	{
		THROW3(mxcore::IOException, std::string("Open file: [") + srcFilepath + "]", mxos::getLastError());
	}

	return new HttpUploadFileInFile(name, displayFilepath, stream, srcFilepath,
			isTemp);
}

HttpUploadFileInFile* HttpFileItem::makeInFileUploadFileItem(
		HttpUploadFileInMemory* inmemory, const std::string& name,
		const std::string& displayFilepath, const std::string& srcFilepath,
		bool isTemp) throw (mxcore::IOException&)
{
	FILE* stream = ::fopen(srcFilepath.c_str(), "rw");

	if (NULL == stream)
	{
		THROW3(mxcore::IOException, std::string("Open file: [") + srcFilepath + "]", mxos::getLastError());
	}

	HttpUploadFileInFile* inFile = new HttpUploadFileInFile(name,
			displayFilepath, stream, srcFilepath, isTemp);

	try
	{
		char buf[4096];
		inmemory->flip();

		int r = inmemory->read(buf, 4096);

		while (r > 0)
		{
			inFile->write(buf, r);
			r = inmemory->read(buf, 4096);
		}

		return inFile;
	} catch (mxcore::IOException& e)
	{
		delete inFile;
		throw e;
	}
}

HttpFormFieldItem::HttpFormFieldItem(const std::string& name) :
	HttpFileItem(HttpFileItem::eFormField, name, new HttpStreamInMemory(""))
{
}

HttpFormFieldItem::~HttpFormFieldItem(void)
{
}

HttpUploadFileItem::HttpUploadFileItem(int type, const std::string& name,
		const std::string& displayFilepath, HttpStream* stream) :
	HttpFileItem(HttpFileItem::eUploadFile, name, stream), type_(type),
			displayFilepath_(displayFilepath)
{
}

HttpUploadFileItem::~HttpUploadFileItem(void)
{
}

HttpUploadFileInMemory::HttpUploadFileInMemory(const std::string& name,
		const std::string& displayFilepath) :
			HttpUploadFileItem(HttpUploadFileItem::eInMemory, name,
					displayFilepath, new HttpStreamInMemory(""))
{
}

HttpUploadFileInMemory::~HttpUploadFileInMemory(void)
{
	close();
}

HttpUploadFileInFile::HttpUploadFileInFile(const std::string& name,
		const std::string& displayFilepath, FILE* stream,
		const std::string& srcFilepath, bool isTemp) :
			HttpUploadFileItem(HttpUploadFileItem::eInMemory, name,
					displayFilepath, new HttpStreamInfile(stream)),
			removeFileOnClose_(isTemp), srcFilepath_(srcFilepath)
{
}

HttpUploadFileInFile::~HttpUploadFileInFile(void)
{
	close();
}

void HttpUploadFileInFile::closeImpl(void)
{
	if (removeFileOnClose_)
	{
		::remove(srcFilepath_.c_str());
	}
}

HttpMultipartBody::HttpMultipartBody(const std::string& boundary) :
	boundary_(boundary)
{
}

HttpMultipartBody::~HttpMultipartBody()
{
	close();
}

bool HttpMultipartBody::isMultipartBody(void) const
{
	return true;
}

void HttpMultipartBody::deleteItem(int index)
{
	MX_ASSERT(index >= 0 && index < (int)items_.size());
	delete removeItem(index);
}

HttpMultipartBody* HttpMultipartBody::makeMultipartBody(
		const std::string& boundary)
{
	return new HttpMultipartBody(boundary);
}

void HttpMultipartBody::closeImpl(void)
{
	while (getItemCount() > 0)
	{
		deleteItem(0);
	}
}

} // namespace mxhttp
