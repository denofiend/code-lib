/*
 *        Name: Real.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_REAL_H__
#define __MXCORE_REAL_H__

#include <string>
#include <mxcore/Assert.h>
#include <mxcore/NumberFormatException.h>

namespace mxcore
{

template<class ValueT>
struct RealConvert
{
};

template<>
struct RealConvert<float>
{
	std::string operator()(float value, int n)
	{
		char tmp[64] =
		{ 0 };
		char fmt[16] = "";

		if (n <= 0)
		{
			n = 4;
		}

		if (n > 10)
		{
			n = 10;
		}

		::sprintf(fmt, "%%.%df", n);
		::snprintf(tmp, 63, fmt, value);
		return tmp;
	}

	float operator()(const char* str)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		float value = ::strtof(str, &endPtr);

		if (NULL == endPtr || '\0' != *endPtr)
		{
			THROW2(NumberFormatException, std::string("Invalid float string:[") + str + "]");
		}

		return value;
	}

	float operator()(const std::string& str)
	{
		return operator()(str.c_str());
	}
};

template<>
struct RealConvert<double>
{
	std::string operator()(double value, int n)
	{
		char tmp[64] =
		{ 0 };
		char fmt[16] = "";

		if (n <= 0)
		{
			n = 4;
		}

		if (n > 10)
		{
			n = 10;
		}

		::sprintf(fmt, "%%.%df", n);
		::snprintf(tmp, 63, fmt, value);
		return tmp;
	}

	double operator()(const char* str)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		double value = ::strtod(str, &endPtr);

		if (NULL == endPtr || '\0' != *endPtr)
		{
			THROW2(NumberFormatException, std::string("Invalid double string:[") + str + "]");
		}

		return value;
	}

	double operator()(const std::string& str)
	{
		return operator()(str.c_str());
	}
};

template<class T>
class Real
{
public:
	template<class ValueT>
	Real(ValueT value) :
		value_(value)
	{
	}

	template<class ValueT>
	Real(const Real& src) :
		value_(src.value())
	{
	}

	~Real(void)
	{
	}

	template<class ValueT>
	Real& operator =(ValueT value)
	{
		value_ = value;
		return *this;
	}

	template<class ValueT>
	Real& operator =(const Real<ValueT> value)
	{
		value_ = value.value();
	}

	template<class ValueT>
	Real operator +(ValueT value) const
	{
		return Real(value_ + value);
	}

	template<class ValueT>
	Real& operator +=(ValueT value)
	{
		value_ += value;
		return *this;
	}

	template<class ValueT>
	Real operator -(ValueT value) const
	{
		return Real(value_ - value);
	}

	template<class ValueT>
	Real& operator -=(ValueT value)
	{
		value_ -= value;
		return *this;
	}

	template<class ValueT>
	Real operator /(ValueT value) const
	{
		return Real(value_ / value);
	}

	template<class ValueT>
	Real& operator /=(ValueT value)
	{
		value_ /= value;
		return *this;
	}

	template<class ValueT>
	Real operator *(ValueT value) const
	{
		return Real(value_ * value);
	}

	template<class ValueT>
	Real& operator *=(ValueT value)
	{
		value_ *= value;
		return *this;
	}

	std::string toString(int n = 0) const
	{
		RealConvert<T> convert;
		return convert(value_, n);
	}

	static Real fromString(const char* str)
	{
		RealConvert<T> convert;
		if (NULL == str || *str == '\0')
		{
			THROW2(NumberFormatException, std::string("Empty real string"));
		}
		return convert(str);
	}

	static Real fromString(const std::string& str)
	{
		RealConvert<T> convert;
		if (str.empty())
		{
			THROW2(NumberFormatException, std::string("Empty real string"));
		}
		return convert(str);
	}

	T getValue(void) const
	{
		return value_;
	}

	void setValue(T value)
	{
		value_ = value;
	}

	operator T(void) const
	{
		return value_;
	}
private:
	T value_;
};

typedef Real<float> Float;
typedef Real<double> Double;

} // namespace mxcore

#endif /* __MXCORE_REAL_H__ */
