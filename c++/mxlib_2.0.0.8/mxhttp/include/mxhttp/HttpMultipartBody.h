/*
 *        Name: HttpMultipartBody.h
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPMULTIPARTBODY_H__
#define __MXHTTP_HTTPMULTIPARTBODY_H__

#include <mxhttp/HttpBody.h>
#include <mxhttp/HttpStream.h>
#include <mxhttp/HttpPairList.h>

namespace mxhttp
{

class HttpFormFieldItem;
class HttpUploadFileInMemory;
class HttpUploadFileInFile;

class HttpFileItem
{
private:
	HttpFileItem(const HttpFileItem&);
	HttpFileItem& operator =(const HttpFileItem&);
protected:
	enum
	{
		eFormField, eUploadFile
	};
	HttpFileItem(int type, const std::string& name, HttpStream* stream);
public:
	virtual ~HttpFileItem(void);

	bool isFormField(void) const;
	bool isUploadFile(void) const;

	const std::string& getName(void) const;

	HttpHeaderList& getHeaders(void);
	const HttpHeaderList& getHeaders(void) const;

	void close(void);
	bool isClosed(void) const;

	void flip(void);
	int64_t getContentLength(void) const;

	int read(void* buf, int len) throw (mxcore::IOException&);
	int write(const void* buf, int len) throw (mxcore::IOException&);

	static HttpFormFieldItem* makeFormFieldItem(const std::string& name);

	static HttpUploadFileInMemory* makeInMemoryUploadFileItem(
			const std::string& name, const std::string& displayFilepath);

	static HttpUploadFileInFile* makeInFileUploadFileItem(
			const std::string& name, const std::string& displayFilepath,
			const std::string& srcFilepath, bool isTemp)
			throw (mxcore::IOException&);

	static HttpUploadFileInFile* makeInFileUploadFileItem(
			HttpUploadFileInMemory* inmemory, const std::string& name,
			const std::string& displayFilepath, const std::string& srcFilepath,
			bool isTemp) throw (mxcore::IOException&);
protected:
	virtual void closeImpl(void);
	HttpStream* getStream(void) const;
private:
	int type_;

	std::string name_;
	HttpStream* stream_;

	HttpHeaderList headers_;
};

inline const std::string& HttpFileItem::getName(void) const
{
	return name_;
}

inline HttpHeaderList& HttpFileItem::getHeaders(void)
{
	return headers_;
}

inline const HttpHeaderList& HttpFileItem::getHeaders(void) const
{
	return headers_;
}

inline void HttpFileItem::close(void)
{
	if (!isClosed())
	{
		stream_->close();
		closeImpl();
	}
}

inline bool HttpFileItem::isClosed(void) const
{
	return stream_->isClosed();
}

inline bool HttpFileItem::isFormField(void) const
{
	return eFormField == type_;
}

inline bool HttpFileItem::isUploadFile(void) const
{
	return eUploadFile == type_;
}

inline void HttpFileItem::flip(void)
{
	MX_ASSERT(!stream_->isClosed());
	stream_->flip();
}

inline int64_t HttpFileItem::getContentLength(void) const
{
	MX_ASSERT(!stream_->isClosed());
	return stream_->getContentLength();
}

inline int HttpFileItem::read(void* buf, int len) throw (mxcore::IOException&)
{
	MX_ASSERT(!stream_->isClosed());
	MX_ASSERT(NULL != buf && len > 0);
	return stream_->read(buf, len);
}

inline int HttpFileItem::write(const void* buf, int len)
		throw (mxcore::IOException&)
{
	MX_ASSERT(!stream_->isClosed());
	MX_ASSERT(NULL != buf && len > 0);
	return stream_->write(buf, len);
}

inline HttpStream* HttpFileItem::getStream(void) const
{
	return stream_;
}

class HttpFormFieldItem: public HttpFileItem
{
	friend class HttpFileItem;
private:
	HttpFormFieldItem(const HttpFormFieldItem&);
	HttpFormFieldItem& operator =(const HttpFormFieldItem&);

	HttpFormFieldItem(const std::string& name);
public:
	virtual ~HttpFormFieldItem(void);

	const std::string& getValue(void) const;
};

inline const std::string& HttpFormFieldItem::getValue(void) const
{
	return ((HttpStreamInMemory*) getStream())->getContent();
}

class HttpUploadFileItem: public HttpFileItem
{
	friend class HttpFileItem;
private:
	HttpUploadFileItem(const HttpUploadFileItem&);
	HttpUploadFileItem& operator =(const HttpUploadFileItem&);
protected:
	enum
	{
		eInMemory, eInFile
	};

	HttpUploadFileItem(int type, const std::string& name,
			const std::string& displayFilepath, HttpStream* stream);
public:
	virtual ~HttpUploadFileItem(void);

	bool isInFile(void) const;
	bool isInMemory(void) const;

	const std::string& getDisplayFilepath(void) const;
	void setDisplayFilepath(const std::string& filepath);
private:
	int type_;
	std::string displayFilepath_;
};

inline bool HttpUploadFileItem::isInFile(void) const
{
	return eInFile == type_;
}

inline bool HttpUploadFileItem::isInMemory(void) const
{
	return eInMemory == type_;
}

inline const std::string& HttpUploadFileItem::getDisplayFilepath(void) const
{
	return displayFilepath_;
}

inline void HttpUploadFileItem::setDisplayFilepath(const std::string& filepath)
{
	displayFilepath_ = filepath;
}

class HttpUploadFileInMemory: public HttpUploadFileItem
{
	friend class HttpFileItem;
private:
	HttpUploadFileInMemory(const HttpUploadFileInMemory&);
	HttpUploadFileInMemory& operator =(const HttpUploadFileInMemory&);

	HttpUploadFileInMemory(const std::string& name,
			const std::string& displayFilepath);
public:
	virtual ~HttpUploadFileInMemory(void);
};

class HttpUploadFileInFile: public HttpUploadFileItem
{
	friend class HttpFileItem;
private:
	HttpUploadFileInFile(const HttpUploadFileInFile&);
	HttpUploadFileInFile& operator =(const HttpUploadFileInFile&);

	HttpUploadFileInFile(const std::string& name,
			const std::string& displayFilepath, FILE* stream,
			const std::string& srcFilepath, bool isTemp);
public:
	virtual ~HttpUploadFileInFile(void);
protected:
	virtual void closeImpl(void);
private:
	bool removeFileOnClose_;
	std::string srcFilepath_;
};

class HttpMultipartBody: public HttpBody
{
private:
	HttpMultipartBody(const HttpMultipartBody&);
	HttpMultipartBody& operator =(const HttpMultipartBody&);
protected:
	HttpMultipartBody(const std::string& boundary);
public:
	virtual ~HttpMultipartBody(void);

	virtual bool isMultipartBody(void) const;

	static HttpMultipartBody* makeMultipartBody(const std::string& boundary);

	const std::string& getBoundary(void) const;
	void setBoundary(const std::string& boundary);

	int getItemCount(void) const;
	void addItem(HttpFileItem* item);
	HttpFileItem* getItem(int index) const;
	HttpFileItem* removeItem(int index);
	void deleteItem(int index);

	HttpUploadFileItem* getFirstUploadFileItem(void) const;
protected:
	virtual void closeImpl(void);
private:
	std::string boundary_;
	std::vector<HttpFileItem*> items_;
};

inline const std::string& HttpMultipartBody::getBoundary(void) const
{
	return boundary_;
}

inline void HttpMultipartBody::setBoundary(const std::string& boundary)
{
	boundary_ = boundary;
}

inline int HttpMultipartBody::getItemCount(void) const
{
	return (int) items_.size();
}

inline void HttpMultipartBody::addItem(HttpFileItem* item)
{
	items_.push_back(item);
}

inline HttpFileItem* HttpMultipartBody::getItem(int index) const
{
	MX_ASSERT(index >= 0 && index < (int)items_.size());
	return items_[index];
}

inline HttpUploadFileItem* HttpMultipartBody::getFirstUploadFileItem(void) const
{
	for (int i = 0; i < (int) items_.size(); i++)
	{
		if (items_[i]->isUploadFile())
		{
			return (HttpUploadFileItem*) items_[i];
		}
	}

	return NULL;
}

inline HttpFileItem* HttpMultipartBody::removeItem(int index)
{
	MX_ASSERT(index >= 0 && index < (int)items_.size());
	HttpFileItem* item = items_[index];
	items_.erase(items_.begin() + index);
	return item;
}

} // namespace mxhttp

#endif /* __MXHTTP_HTTPMULTIPARTBODY_H__ */
