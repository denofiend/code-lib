/*
 *        Name: Number.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_NUMBER_H__
#define __MXCORE_NUMBER_H__

#include <stdint.h>
#include <iostream>
#include <mxcore/Assert.h>
#include <mxcore/NumberFormatException.h>

namespace mxcore
{

template<class ValueT>
struct NumberConvert
{
};

template<>
struct NumberConvert<int8_t>
{
	std::string operator()(int8_t value)
	{
		char tmp[32] =
		{ 0 };
		::snprintf(tmp, 31, "%d", value);
		return tmp;
	}

	int8_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		long value = ::strtol(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (int8_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid int8_t string:[") + str + "]");
		}

		return (int8_t) value;
	}

	int8_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<>
struct NumberConvert<uint8_t>
{
	std::string operator()(uint8_t value)
	{
		char tmp[32] = "0";
		::snprintf(tmp, 31, "%u", value);
		return tmp;
	}

	uint8_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		unsigned long value = ::strtoul(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (uint8_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid uint8_t string:[") + str + "]");
		}

		return (uint8_t) value;
	}

	uint8_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<>
struct NumberConvert<int16_t>
{
	std::string operator()(int16_t value)
	{
		char tmp[32] = "0";
		::snprintf(tmp, 31, "%d", value);
		return tmp;
	}

	int16_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		long value = ::strtol(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (int16_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid int16_t string:[") + str + "]");
		}

		return (int16_t) value;
	}

	int16_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<>
struct NumberConvert<uint16_t>
{
	std::string operator()(uint16_t value)
	{
		char tmp[32] = "0";
		::snprintf(tmp, 31, "%u", value);
		return tmp;
	}

	uint16_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		unsigned long value = ::strtoul(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (uint16_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid uint16_t string:[") + str + "]");
		}

		return (uint16_t) value;
	}

	uint16_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<>
struct NumberConvert<int32_t>
{
	std::string operator()(int32_t value)
	{
		char tmp[32] = "0";
		::snprintf(tmp, 31, "%d", value);
		return tmp;
	}

	int32_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		long value = ::strtol(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (int32_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid int32_t string:[") + str + "]");
		}

		return (int32_t) value;
	}

	int32_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<>
struct NumberConvert<uint32_t>
{
	std::string operator()(uint32_t value)
	{
		char tmp[32] = "0";
		::snprintf(tmp, 31, "%u", value);
		return tmp;
	}

	uint32_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		unsigned long value = ::strtoul(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (uint32_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid uint32_t string:[") + str + "]");
		}

		return (uint32_t) value;
	}

	uint32_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<>
struct NumberConvert<int64_t>
{
	std::string operator()(int64_t value)
	{
		char tmp[32] = "0";
		::snprintf(tmp, 31, "%lld", value);
		return tmp;
	}

	int64_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		long long value = ::strtoll(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (int64_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid int64_t string:[") + str + "]");
		}

		return (int64_t) value;
	}

	int64_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<>
struct NumberConvert<uint64_t>
{
	std::string operator()(uint64_t value)
	{
		char tmp[32] = "0";
		::snprintf(tmp, 31, "%llu", value);
		return tmp;
	}

	uint64_t operator()(const char* str, int base)
	{
		char* endPtr = NULL;

		MX_ASSERT(NULL != str);

		unsigned long long value = ::strtoull(str, &endPtr, base);

		if (NULL == endPtr || '\0' != *endPtr || (value != (uint64_t) value))
		{
			THROW2(NumberFormatException, std::string("Invalid uint64_t string:[") + str + "]");
		}

		return (uint64_t) value;
	}

	uint64_t operator()(const std::string& str, int base)
	{
		return operator()(str.c_str(), base);
	}
};

template<class T>
class Number
{
public:
	Number(void) :
		value_(0)
	{
	}

	template<class ValueT>
	Number(ValueT value) :
		value_((T) value)
	{
	}

	template<class ValueT>
	Number(const Number<ValueT>& src) :
		value_((T) src.value())
	{
	}

	Number(const Number& src) :
		value_(src.value_)
	{
	}

	~Number(void)
	{
	}

	operator T(void) const
	{
		return value_;
	}

	friend std::istream& operator <<(std::istream& in, const Number& value)
	{
		in << (T) value.value_;
		return in;
	}

	friend std::ostream& operator >>(std::ostream& out, const Number& value)
	{
		out << (T) value.value_;
		return out;
	}

	friend std::string& operator +=(std::string& str, const Number& value)
	{
		str.append(value.toString());
		return str;
	}

	friend std::string operator +(const std::string& str, const Number& value)
	{
		std::string ret(str);
		return ret += value;
	}

	template<class ValueT>
	Number& operator =(ValueT value)
	{
		value_ = value;
	}

	template<class ValueT>
	Number& operator =(const Number<ValueT> value)
	{
		value_ = value.getValue();
	}

	template<class ValueT>
	Number operator +(ValueT value) const
	{
		return Number(value_ + value);
	}

	template<class ValueT>
	Number& operator +=(ValueT value)
	{
		value_ += value;
		return *this;
	}

	template<class ValueT>
	Number operator -(ValueT value) const
	{
		return Number(value_ - value);
	}

	template<class ValueT>
	Number& operator -=(ValueT value)
	{
		value_ -= value;
		return *this;
	}

	template<class ValueT>
	Number operator /(ValueT value) const
	{
		return Number(value_ / value);
	}

	template<class ValueT>
	Number& operator /=(ValueT value)
	{
		value_ /= value;
		return *this;
	}

	template<class ValueT>
	Number operator *(ValueT value) const
	{
		return Number(value_ * value);
	}

	template<class ValueT>
	Number& operator *=(ValueT value)
	{
		value_ *= value;
		return *this;
	}

	T getValue(void) const
	{
		return value_;
	}

	void setValue(T value) const
	{
		value_ = value;
	}

	std::string toString(void) const
	{
		NumberConvert<T> convert;
		return convert(value_);
	}

	static Number fromString(const std::string& str, int base = 10)
	{
		NumberConvert<T> convert;
		if (str.empty())
		{
			THROW2(NumberFormatException, std::string("Empty numberic string"));
		}
		return convert(str, base);
	}

	static Number fromString(const char* str, int base = 10)
			throw (NumberFormatException&)
	{
		NumberConvert<T> convert;
		if (NULL == str || *str == '\0')
		{
			THROW2(NumberFormatException, std::string("Empty numberic string"));
		}
		return convert(str, base);
	}
protected:
	T value_;
};

typedef Number<int8_t> Byte;
typedef Number<uint8_t> UByte;
typedef Number<int16_t> Short;
typedef Number<uint16_t> UShort;
typedef Number<int32_t> Integer;
typedef Number<uint32_t> UInteger;
typedef Number<int64_t> Long;
typedef Number<uint64_t> ULong;

} // namespace mxcore

#endif /* __MXCORE_NUMBER_H__ */
