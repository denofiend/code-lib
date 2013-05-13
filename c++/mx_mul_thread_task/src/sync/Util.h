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

static bool decode(const Json::Value& value, const char* key, std::string& dst)
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

static bool decode(const Json::Value& value, const char* key, uint32_t& dst)
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

static bool decode(const Json::Value& value, const char* key, uint16_t& dst)
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

static bool decode(const Json::Value& value, const char* key, int& dst)
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

static bool decode(const Json::Value& value, const char* key, uint64_t& dst)
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

static bool decodeDefault(const Json::Value& value, const char* key, std::string& dst,
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
static bool decodeNull(const Json::Value & value, const char *key, std::string & dst)
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

static bool decodeNull(const Json::Value & value, const char *key, uint32_t & dst)
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

static bool decodeDefault(const Json::Value& value, const char* key, uint32_t& dst,
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

static bool decodeDefault(const Json::Value& value, const char* key, uint64_t& dst,
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
static const char CCH[] =
		"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static std::string getRandomString()
{
	int sz = 0;
	while (0 == sz)
		sz = rand() / (RAND_MAX / (sizeof(CCH)));

	std::string nickname;
	for (int i = 0; i < sz; ++i)
	{
		int x = rand() / (RAND_MAX / (sizeof(CCH) - 1));
		nickname.push_back(CCH[x]);
	}
	return nickname + ".mx";
}

}
#endif /* UTIL_H_ */
