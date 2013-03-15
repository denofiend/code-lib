/*
 * IniParser.h
 *
 *  Created on: 2011-4-27
 *      Author: zhaojiangang
 */

#ifndef __MXCORE_INIPARSER_H__
#define __MXCORE_INIPARSER_H__

#include <map>
#include <vector>
#include <string>

namespace mxcore
{

class IniParser
{
public:
	IniParser();
	virtual ~IniParser();

	int parseFile(const std::string& iniFile);

	int parseContent(const char* str, int len);

	int parseContent(const std::string& str);

	bool getValue(const std::string& sectionName, const std::string& subName,
			std::string& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			int16_t& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			uint16_t& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			int32_t& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			uint32_t& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			int64_t& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			uint64_t& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			float& value) const;

	bool getValue(const std::string& sectionName, const std::string& subName,
			double& value) const;

	const std::vector<std::pair<std::string, std::string> >* getSection(
			const std::string& name) const;

	const std::map<std::string,
			std::vector<std::pair<std::string, std::string> > >&
	getSectionMap() const;
private:
	bool igoncase_;

	std::map<std::string, std::vector<std::pair<std::string, std::string> > >
			sectionMap_;
};

} // namespace mxcore

#endif /* __MXCORE_INIPARSER_H__ */
