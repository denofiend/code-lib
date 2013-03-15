/*
 * Xml.h
 *
 *  Created on: 2011-3-7
 *      Author: zhaojiangang
 */

#ifndef __MXCORE_XML_H__
#define __MXCORE_XML_H__

#include <stdint.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <mxcore/Assert.h>
#include <mxcore/Real.h>
#include <mxcore/Number.h>
#include <mxcore/XmlException.h>
#include <mxcore/NumberFormatException.h>

namespace mxcore
{

template<bool bManager>
class XmlDocT;

typedef XmlDocT<false> XmlDocHandle;
typedef XmlDocT<true> XmlDoc;

class XmlNode
{
public:
	XmlNode(xmlNodePtr node = NULL);
	XmlNode(const XmlNode& src);
	XmlNode& operator =(const XmlNode& src);
	~XmlNode(void);

	operator xmlNodePtr(void) const;
	xmlNodePtr getNodePtr(void) const;

	bool isNull(void) const;
	bool isElement(void) const;
	bool isComment(void) const;
	bool isText(void) const;
	bool isAttr(void) const;
	bool isDocument(void) const;

	xmlElementType getType(void) const;

	const char* getName(void) const;
	void setName(const char* name);

	XmlDocHandle getDoc(void) const;
	void remove(void);
public:
	XmlNode firstChild(void) const;
	XmlNode lastChild(void) const;
	XmlNode nextSibling(void) const;
	XmlNode prevSibling(void) const;
protected:
	xmlNodePtr nodePtr;
};

class XmlElement: public XmlNode
{
public:
	XmlElement(xmlElementPtr eltPtr = NULL);
	XmlElement(const XmlElement& src);
	XmlElement& operator =(const XmlElement& src);
	~XmlElement(void);

	operator xmlElementPtr(void) const;
	xmlElementPtr getElementPtr(void) const;

	bool hasAttr(void) const;
	bool hasAttr(const char* attrName) const;

	int16_t getShort(void) const throw (NumberFormatException&);
	uint16_t getUShort(void) const throw (NumberFormatException&);

	int32_t getInt(void) const throw (NumberFormatException&);
	uint32_t getUInt(void) const throw (NumberFormatException&);

	int64_t getLong(void) const throw (NumberFormatException&);
	uint64_t getULong(void) const throw (NumberFormatException&);

	float getFloat(void) const throw (NumberFormatException&);
	std::string getString(void) const;

	XmlElement& setShort(int16_t value);
	XmlElement& setUShort(uint16_t value);
	XmlElement& setInt(int32_t value);
	XmlElement& setUInt(uint32_t value);
	XmlElement& setLong(int64_t value);
	XmlElement& setULong(uint64_t value);
	XmlElement& setFloat(float value, int prec = 4);
	XmlElement& setString(const char* str);
	XmlElement& setString(const std::string& str);

	XmlElement& addChild(const XmlNode& child);
	XmlElement& addSibling(const XmlNode& sibling);

	int removeAttr(const char* attrName);

	int16_t getAttrShort(const char* attrName) const
			throw (NumberFormatException&);
	uint16_t getAttrUShort(const char* attrName) const
			throw (NumberFormatException&);
	int32_t getAttrInt(const char* attrName) const
			throw (NumberFormatException&);
	uint32_t getAttrUInt(const char* attrName) const
			throw (NumberFormatException&);
	int64_t getAttrLong(const char* attrName) const
			throw (NumberFormatException&);
	uint64_t getAttrULong(const char* attrName) const
			throw (NumberFormatException&);
	float getAttrFloat(const char* attrName) const
			throw (NumberFormatException&);
	std::string getAttrString(const char* attrName) const;

	XmlElement& setAttrShort(const char* attrName, int16_t value)
			throw (XmlException&);
	XmlElement& setAttrUShort(const char* attrName, uint16_t value)
			throw (XmlException&);
	XmlElement& setAttrInt(const char* attrName, int32_t value)
			throw (XmlException&);
	XmlElement& setAttrUInt(const char* attrName, uint32_t value)
			throw (XmlException&);
	XmlElement& setAttrLong(const char* attrName, int64_t value)
			throw (XmlException&);
	XmlElement& setAttrULong(const char* attrName, uint64_t value)
			throw (XmlException&);
	XmlElement& setAttrFloat(const char* attrName, float value, int prec)
			throw (XmlException&);
	XmlElement& setAttrString(const char* attrName, const char* str)
			throw (XmlException&);
	XmlElement& setAttrString(const char* attrName, const std::string& str)
			throw (XmlException&);

	XmlElement getFirstChild(void) const;
	XmlElement getLastChild(void) const;
	XmlElement getNextSibling(void) const;
	XmlElement getPrevSibling(void) const;

	XmlElement getFirstChildByName(const char* name) const;
	XmlElement getNextSiblingByName(const char* name) const;

	XmlElement addChild(XmlElement child);
	XmlElement appendChild(XmlElement child);
	XmlElement appendChild(const char* name);

	void removeChildSelf(void);
	void removeChild(XmlElement elt);
};

template<bool bManager>
class XmlDocT: public XmlNode
{
public:
	XmlDocT(xmlDocPtr ptr = NULL) :
		XmlNode((xmlNodePtr) ptr)
	{
	}

	~XmlDocT(void)
	{
		if (bManager)
		{
			freeDoc();
		}
	}

	void attach(xmlDocPtr ptr)
	{
		MX_ASSERT(isNull());
		nodePtr = ptr;
	}

	xmlDocPtr detach(void)
	{
		if (!isNull())
		{
			xmlDocPtr tmp = (xmlDocPtr) nodePtr;
			nodePtr = NULL;
			return tmp;
		}
		return (xmlDocPtr) NULL;
	}

	operator xmlDocPtr(void) const
	{
		return (xmlDocPtr) nodePtr;
	}

	XmlDocT& parseDoc(const char* doc) throw (XmlException&)
	{
		MX_ASSERT(isNull());
		nodePtr = (xmlNodePtr) xmlParseDoc((const xmlChar*) doc);

		if (NULL == nodePtr)
		{
			THROW2(XmlException, "Parse doc failed");
		}
		return *this;
	}

	XmlDocT& parseDoc(const char* buf, int len) throw (XmlException&)
	{
		MX_ASSERT(isNull());
		nodePtr = (xmlNodePtr) xmlParseMemory(buf, len);

		if (NULL == nodePtr)
		{
			THROW2(XmlException, "Parse doc failed");
		}
		return *this;
	}

	XmlDocT& parseFile(const char* fileName) throw (XmlException&)
	{
		MX_ASSERT(isNull());
		nodePtr = (xmlNodePtr) xmlParseFile(fileName);

		if (NULL == nodePtr)
		{
			THROW2(XmlException, std::string("Parse failed failed: ") + fileName);
		}
		return *this;
	}

	XmlDocT& createNewDoc(const char* version = "1.0") throw (XmlException&)
	{
		MX_ASSERT(isNull());
		nodePtr = (xmlNodePtr) xmlNewDoc((xmlChar*) version);

		if (NULL == nodePtr)
		{
			THROW2(XmlException, "Create new doc failed");
		}
		return *this;
	}

	XmlDocT& save(const char* fileName, const char* encoding = NULL,
			int format = 0) throw (XmlException&)
	{
		MX_ASSERT(!isNull());

		if (NULL == encoding)
		{
			if (0 != xmlSaveFormatFile(fileName, (xmlDocPtr) nodePtr, format))
			{
				THROW2(XmlException, std::string("Save to file: [") + fileName + "] failed");
			}
		}
		else
		{
			if (0 != xmlSaveFormatFileEnc(fileName, (xmlDocPtr) nodePtr,
					encoding, format))
			{
				THROW2(XmlException, std::string("Save to file: [") + fileName + "] failed");
			}
		}
		return *this;
	}

	XmlDocT& dump(std::string& str, const char* encoding = "utf-8",
			int format = 0) throw (XmlException&)
	{
		xmlChar* docTxt = NULL;
		int docTxtLen = 0;

		MX_ASSERT(!isNull());

		xmlDocDumpFormatMemoryEnc((xmlDocPtr) nodePtr, &docTxt, &docTxtLen,
				encoding, format);

		if (NULL == docTxt)
		{
			THROW2(XmlException, "Dump failed");
		}

		str.append((const char*) docTxt);

		xmlFree(docTxt);

		return *this;
	}

	XmlDocT& freeDoc(void)
	{
		if (NULL != nodePtr)
		{
			xmlFreeDoc((xmlDocPtr) nodePtr);
			nodePtr = NULL;
		}
		return *this;
	}

	XmlElement getRootElement(void) const
	{
		MX_ASSERT(!isNull());
		return (xmlElementPtr) xmlDocGetRootElement((xmlDocPtr) nodePtr);
	}

	XmlDocT& setRootElement(XmlElement elt)
	{
		MX_ASSERT(!isNull());
		xmlDocSetRootElement((xmlDocPtr) nodePtr, (xmlNodePtr) elt);
		return *this;
	}

	const char* getEncoding(void) const
	{
		MX_ASSERT(!isNull());
		return (const char*) ((xmlDocPtr) nodePtr)->encoding;
	}

	const char* getVersion(void) const
	{
		MX_ASSERT(!isNull());
		return (const char*) ((xmlDocPtr) nodePtr)->version;
	}

	const char* getUrl(void) const
	{
		MX_ASSERT(!isNull());
		return (const char*) ((xmlDocPtr) nodePtr)->URL;
	}

	XmlElement createElement(const char* name, const char* val)
			throw (XmlException&)
	{
		MX_ASSERT(!isNull());
		MX_ASSERT(NULL != name && *name != '\0');

		XmlElement elt = (xmlElementPtr) xmlNewDocNode((xmlDocPtr) nodePtr,
				NULL, (const xmlChar*) name, (const xmlChar*) val);

		if (elt.isNull())
		{
			THROW2(XmlException, "Create element failed");
		}

		return elt;
	}
};

} // namespace mxcore

#endif /* __MXCORE_XML_H__ */
