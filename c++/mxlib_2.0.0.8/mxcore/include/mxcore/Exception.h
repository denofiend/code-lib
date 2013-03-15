/*
 *        Name: Exception.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_EXCEPTION_H__
#define __MXCORE_EXCEPTION_H__

#include <string>

#define THROW1(E) \
	throw E("", 0, __FILE__, __LINE__)

#define THROW2(E, MSG) \
	throw E((MSG), 0, __FILE__, __LINE__)

#define THROW3(E, MSG, ERR) \
	throw E((MSG), (ERR), __FILE__, __LINE__)

namespace mxcore
{

class Exception
{
public:
	Exception(const std::string& message, int errorCode,
			const std::string& filename, int line);
	Exception(const Exception& src);
	virtual ~Exception(void);

	Exception& operator =(const Exception& other);

	virtual const std::string& name(void) const;

	void setMessage(const std::string& message);
	const std::string& getMessage(void) const;

	void setErrorCode(int errorCode);
	int getErrorCode(void) const;

	void setFilename(const std::string& filename);
	const std::string& getFilename(void) const;

	void setLineNumber(int lineNumber);
	int getLineNumber(void) const;
private:
	std::string message_;
	int errorCode_;

	std::string filename_;
	int line_;
};

inline void Exception::setMessage(const std::string& message)
{
	message_ = message;
}

inline const std::string& Exception::getMessage(void) const
{
	return message_;
}

inline void Exception::setErrorCode(int errorCode)
{
	errorCode_ = errorCode;
}

inline int Exception::getErrorCode(void) const
{
	return errorCode_;
}

inline void Exception::setFilename(const std::string& filename)
{
	filename_ = filename;
}

inline const std::string& Exception::getFilename(void) const
{
	return filename_;
}

inline void Exception::setLineNumber(int lineNumber)
{
	line_ = lineNumber;
}

inline int Exception::getLineNumber(void) const
{
	return line_;
}

} // namespace mxcore

#endif /* __MXCORE_EXCEPTION_H__ */
