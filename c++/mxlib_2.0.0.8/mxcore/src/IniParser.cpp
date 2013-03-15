/*
 * IniParser.cpp
 *
 *  Created on: 2011-4-27
 *      Author: zhaojiangang
 */

#include <fstream>
#include <iostream>
#include <mxcore/Real.h>
#include <mxcore/Number.h>
#include <mxcore/IniParser.h>
#include <mxcore/StringUtil.h>

namespace mxcore
{

IniParser::IniParser()
{
}

IniParser::~IniParser()
{
}

int IniParser::parseFile(const std::string& iniFile)
{
	sectionMap_.clear();

	std::ifstream in;

	in.open(iniFile.c_str());

	if (in.fail())
	{
		std::cout<<"Open file: "<<iniFile<<std::endl;
		return -1;
	}

	std::string line;
	std::string sectionName = "";

	while (std::getline(in, line))
	{
		mxcore::StringUtil::trim(line);

		if (line.empty())
		{
			continue;
		}

		if (line[0] == '#')
		{
			continue;
		}

		if (line[0] == '[')
		{
			if (line.length() < 2)
			{
				return -1;
			}

			if (line[line.length() - 1] != ']')
			{
				return -1;
			}

			sectionName = line.substr(1, line.length() - 2);

			if (igoncase_)
			{
				mxcore::StringUtil::toLower(sectionName);
			}
		}
		else if (sectionName.empty())
		{
			return -1;
		}
		else
		{
			size_t pos = line.find('=');

			if (std::string::npos == pos)
			{
				return -1;
			}
			else
			{
				std::string key = line.substr(0, pos);
				std::string value = line.substr(pos + 1);
				mxcore::StringUtil::trim(key);

				if (igoncase_)
				{
					mxcore::StringUtil::toLower(key);
				}
				sectionMap_[sectionName].push_back(std::make_pair(key, value));
			}
		}
	}

	return 0;
}

int IniParser::parseContent(const char* str, int len)
{
	sectionMap_.clear();

	std::string line;
	std::string sectionName = "";

	while (len > 0)
	{
		int n = mxcore::StringUtil::getLine(str, len, line);

		if (n == 0)
		{
			n = len;
			line.clear();
			line.append(str, len);
		}

		len -= n;
		str += n;

		mxcore::StringUtil::trim(line);

		if (line.empty())
		{
			continue;
		}

		if (line[0] == '#')
		{
			continue;
		}

		if (line[0] == '[')
		{
			if (line.length() < 2)
			{
				return -1;
			}

			if (line[line.length() - 1] != ']')
			{
				return -1;
			}

			sectionName = line.substr(1, line.length() - 2);

			if (igoncase_)
			{
				mxcore::StringUtil::toLower(sectionName);
			}
		}
		else if (sectionName.empty())
		{
			return -1;
		}
		else
		{
			size_t pos = line.find('=');

			if (std::string::npos == pos)
			{
				return -1;
			}
			else
			{
				std::string key = line.substr(0, pos);
				std::string value = line.substr(pos + 1);
				mxcore::StringUtil::trim(key);

				if (igoncase_)
				{
					mxcore::StringUtil::toLower(key);
				}

				sectionMap_[sectionName].push_back(std::make_pair(key, value));
			}
		}
	}

	return 0;
}

int IniParser::parseContent(const std::string& str)
{
	return parseContent(str.c_str(), str.size());
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, std::string& value) const
{
	const std::vector<std::pair<std::string, std::string> >* section =
			getSection(sectionName);

	if (NULL == section)
	{
		return false;
	}

	for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
			section->begin(); it != section->end(); it++)
	{
		const std::pair<std::string, std::string>& p = *it;

		if (mxcore::StringUtil::equalNoCase(p.first, subName))
		{
			value = p.second;
			return true;
		}
	}

	return false;
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, int16_t& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = Short::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, uint16_t& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = UShort::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, int32_t& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = Integer::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, uint32_t& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = UInteger::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, int64_t& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = Long::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, uint64_t& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = ULong::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, float& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = Float::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

bool IniParser::getValue(const std::string& sectionName,
		const std::string& subName, double& value) const
{
	std::string strValue;

	if (!getValue(sectionName, subName, strValue))
	{
		return false;
	}

	try
	{
		value = Double::fromString(strValue);
		return true;
	} catch (NumberFormatException& e)
	{
		return false;
	}
}

const std::vector<std::pair<std::string, std::string> >* IniParser::getSection(
		const std::string& name) const
{
	std::string lowerName(name);

	if (igoncase_)
	{
		mxcore::StringUtil::toLower(lowerName);
	}

	std::map<std::string, std::vector<std::pair<std::string, std::string> > >::const_iterator
			it = sectionMap_.find(lowerName);

	if (it == sectionMap_.end())
	{
		return NULL;
	}

	return &it->second;
}

const std::map<std::string, std::vector<std::pair<std::string, std::string> > >& IniParser::getSectionMap() const
{
	return sectionMap_;
}

} // namespace mxcore
