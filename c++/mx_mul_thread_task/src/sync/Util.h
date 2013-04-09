/*
 * Util.h
 *
 *  Created on: 2013-4-9
 *      Author: DenoFiend
 */

#ifndef UTIL_H_
#define UTIL_H_

namespace MxUtil
{

bool decode(const Json::Value& value, const char* key, std::string& dst)
{
	if (value[key].isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::stringValue))
	{
		return false;
	}

	dst = subValue.asString();
	return true;
}

bool decode(const Json::Value& value, const char* key, uint32_t& dst)
{
	if (value[key].isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asUInt();
	return true;
}

bool decode(const Json::Value& value, const char* key, uint16_t& dst)
{
	if (value[key].isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asUInt();
	return true;
}

bool decode(const Json::Value& value, const char* key, int& dst)
{
	if (value[key].isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asInt();
	return true;
}



bool decode(const Json::Value& value, const char* key, uint64_t& dst)
{
	if (value.isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asUInt64();
	return true;
}

bool decodeDefault(const Json::Value& value, const char* key, std::string& dst,
		const std::string& defValue)
{
	if (value.isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull())
	{
		dst = defValue;
		return false;
	}
	else if (!subValue.isConvertibleTo(Json::stringValue))
	{
		return false;
	}

	dst = subValue.asString();
	return true;
}
bool decodeNull(const Json::Value & value, const char *key, std::string & dst)
{
	if (!value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asString();
	return true;
}

bool decodeNull(const Json::Value & value, const char *key, uint32_t & dst)
{
	if (!value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull() || !subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asUInt();
	return true;
}

bool decodeDefault(const Json::Value& value, const char* key, uint32_t& dst,
		uint32_t defValue)
{
	if (value.isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull())
	{
		dst = defValue;
		return false;
	}
	else if (!subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asUInt();
	return true;
}

bool decodeDefault(const Json::Value& value, const char* key, uint64_t& dst,
		uint64_t defValue)
{
	if (value.isNull() || !value.isObject())
	{
		return false;
	}

	const Json::Value& subValue = value[key];

	if (subValue.isNull())
	{
		dst = defValue;
		return false;
	}
	else if (!subValue.isConvertibleTo(Json::uintValue))
	{
		return false;
	}

	dst = subValue.asUInt();
	return true;
}
}
#endif /* UTIL_H_ */
