/*
 *        Name: GenernicFactory.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_GENERNICFACTORY_H__
#define __MXCORE_GENERNICFACTORY_H__

#include <map>
#include <string>
using namespace std;

namespace mxcore
{

template<class ClassType, class ClassIDKey = string> 
class GenernicFactory
{
	typedef ClassType* (*Creator)();
	typedef map<ClassIDKey, Creator> CreatorMap;
	typedef typename map<ClassIDKey, Creator>::iterator CreatorIter;
	typedef typename map<ClassIDKey, Creator>::const_iterator ConstCreatorIter;
	GenernicFactory() {}
	GenernicFactory(const GenernicFactory&);
	GenernicFactory& operator = (const GenernicFactory&);
public:
	void registerCreator(const ClassIDKey& key, Creator creator)
	{
		m_creatorMap[key] = creator;
	}
	
	ClassType* create(const ClassIDKey& key) const
	{
		ConstCreatorIter it = m_creatorMap.find(key);
		
		if (it != m_creatorMap.end())
			return (*(it->second))();
		
		return 0;
	}
	
	static GenernicFactory& getInstance()
	{
		static GenernicFactory s_instance; 
		
		return s_instance;
	}
private:
	CreatorMap m_creatorMap;
};

template < class BaseClass, class ClassType, class ClassIDKey = string>
class RegisterInFactory
{
	static BaseClass* createInstance()
	{
		return static_cast<BaseClass*>(new ClassType);
	}
public:
	RegisterInFactory(const ClassIDKey& classID)
	{
		GenernicFactory<BaseClass, ClassIDKey>::getInstance().registerCreator(classID, createInstance);
	}
};

} // namespace mxcore

#endif // __MXCORE_GENERNICFACTORY_H__
