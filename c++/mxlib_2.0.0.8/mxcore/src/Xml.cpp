/*
 * Xml.cpp
 *
 *  Created on: 2011-3-7
 *      Author: zhaojiangang
 */

#include <mxcore/Xml.h>
#include <mxcore/StringUtil.h>

namespace mxcore
{

class CXmlInit
{
public:
	CXmlInit(void)
	{
		xmlInitParser();
	}

	~CXmlInit(void)
	{
		xmlCleanupParser();
	}
};

static CXmlInit sXmlInit;

XmlNode::XmlNode(xmlNodePtr node) :
	nodePtr(node)
{
}

XmlNode::XmlNode(const XmlNode& src) :
	nodePtr(src.nodePtr)
{
}

XmlNode& XmlNode::operator =(const XmlNode& src)
{
	if (this != &src)
	{
		nodePtr = src.nodePtr;
	}
	return *this;
}

XmlNode::~XmlNode(void)
{
}

XmlNode::operator xmlNodePtr(void) const
{
	return nodePtr;
}

xmlNodePtr XmlNode::getNodePtr(void) const
{
	return nodePtr;
}

bool XmlNode::isNull(void) const
{
	return NULL == nodePtr;
}

bool XmlNode::isElement(void) const
{
	MX_ASSERT(!isNull());
	return XML_ELEMENT_NODE == nodePtr->type;
}

bool XmlNode::isComment(void) const
{
	MX_ASSERT(!isNull());
	return XML_COMMENT_NODE == nodePtr->type;
}

bool XmlNode::isText(void) const
{
	MX_ASSERT(!isNull());
	return XML_TEXT_NODE == nodePtr->type;
}

bool XmlNode::isAttr(void) const
{
	MX_ASSERT(!isNull());
	return XML_COMMENT_NODE == nodePtr->type;
}

bool XmlNode::isDocument(void) const
{
	MX_ASSERT(!isNull());
	return XML_DOCUMENT_NODE == nodePtr->type;
}

xmlElementType XmlNode::getType(void) const
{
	MX_ASSERT(!isNull());
	return nodePtr->type;
}

const char* XmlNode::getName(void) const
{
	MX_ASSERT(!isNull());
	return (const char*) nodePtr->name;
}

void XmlNode::setName(const char* name)
{
	MX_ASSERT(!isNull());
	xmlNodeSetName(nodePtr, (xmlChar*) name);
}

XmlDocHandle XmlNode::getDoc(void) const
{
	MX_ASSERT(!isNull());
	return nodePtr->doc;
}

XmlNode XmlNode::firstChild(void) const
{
	MX_ASSERT(!isNull());
	return nodePtr->children;
}

XmlNode XmlNode::lastChild(void) const
{
	MX_ASSERT(!isNull());
	return nodePtr->last;
}

XmlNode XmlNode::nextSibling(void) const
{
	MX_ASSERT(!isNull());
	return nodePtr->next;
}

XmlNode XmlNode::prevSibling(void) const
{
	MX_ASSERT(!isNull());
	return nodePtr->prev;
}

void XmlNode::remove(void)
{
	MX_ASSERT(!isNull());
	xmlUnlinkNode(nodePtr);
	xmlFreeNode(nodePtr);
	nodePtr = NULL;
}

///////////////////////////////////////////////////////////////////////

XmlElement::XmlElement(xmlElementPtr eltPtr) :
	XmlNode((xmlNodePtr) eltPtr)
{
}

XmlElement::XmlElement(const XmlElement& src) :
	XmlNode(src)
{
}

XmlElement& XmlElement::operator =(const XmlElement& src)
{
	if (this != &src)
	{
		XmlNode::operator =(src);
	}
	return *this;
}

XmlElement::~XmlElement()
{
}

XmlElement::operator xmlElementPtr(void) const
{
	return (xmlElementPtr) nodePtr;
}

xmlElementPtr XmlElement::getElementPtr(void) const
{
	return (xmlElementPtr) nodePtr;
}

bool XmlElement::hasAttr(void) const
{
	MX_ASSERT(!isNull());
	return NULL != nodePtr->properties;
}

bool XmlElement::hasAttr(const char* attrName) const
{
	MX_ASSERT(!isNull());
	return (NULL != xmlHasProp(nodePtr, (const xmlChar*) attrName));
}

int16_t XmlElement::getShort(void) const throw (NumberFormatException&)
{
	std::string str = getString();
	return Short::fromString(str);
}

uint16_t XmlElement::getUShort(void) const throw (NumberFormatException&)
{
	std::string str = getString();
	return UShort::fromString(str);
}

int32_t XmlElement::getInt(void) const throw (NumberFormatException&)
{
	std::string str = getString();
	return Integer::fromString(str);
}

uint32_t XmlElement::getUInt(void) const throw (NumberFormatException&)
{
	std::string str = getString();
	return UInteger::fromString(str);
}

int64_t XmlElement::getLong(void) const throw (NumberFormatException&)
{
	std::string str = getString();
	return Long::fromString(str);
}

uint64_t XmlElement::getULong(void) const throw (NumberFormatException&)
{
	std::string str = getString();
	return ULong::fromString(str);
}

float XmlElement::getFloat(void) const throw (NumberFormatException&)
{
	std::string str = getString();
	return Float::fromString(str);
}

std::string XmlElement::getString(void) const
{
	MX_ASSERT(!isNull());
	char* value = (char*) xmlNodeGetContent(nodePtr);

	if (NULL == value)
	{
		return "";
	}

	std::string ret(value);
	xmlFree(value);
	return ret;
}

XmlElement& XmlElement::setShort(int16_t value)
{
	return setString(Short(value).toString());
}

XmlElement& XmlElement::setUShort(uint16_t value)
{
	return setString(UShort(value).toString());
}

XmlElement& XmlElement::setInt(int32_t value)
{
	return setString(Integer(value).toString());
}

XmlElement& XmlElement::setUInt(uint32_t value)
{
	return setString(UInteger(value).toString());
}

XmlElement& XmlElement::setLong(int64_t value)
{
	return setString(Long(value).toString());
}

XmlElement& XmlElement::setULong(uint64_t value)
{
	return setString(ULong(value).toString());
}

XmlElement& XmlElement::setFloat(float value, int prec)
{
	return setString(Float(value).toString(prec));
}

XmlElement& XmlElement::setString(const char* str)
{
	MX_ASSERT(!isNull());
	xmlNodeSetContent(nodePtr, (xmlChar*) str);
	return *this;
}

XmlElement& XmlElement::setString(const std::string& str)
{
	return setString(str.c_str());
}

XmlElement& XmlElement::addChild(const XmlNode& child)
{
	MX_ASSERT(!isNull());
	MX_ASSERT(!child.isNull());
	xmlAddChild(nodePtr, child);
	return *this;
}

XmlElement& XmlElement::addSibling(const XmlNode& sibling)
{
	MX_ASSERT(!isNull());
	MX_ASSERT(!sibling.isNull());
	xmlAddSibling(nodePtr, sibling);
	return *this;
}

int XmlElement::removeAttr(const char* attrName)
{
	MX_ASSERT(!isNull());
	return xmlUnsetProp(nodePtr, (const xmlChar*) attrName);
}

int16_t XmlElement::getAttrShort(const char* attrName) const
		throw (NumberFormatException&)
{
	std::string str = getAttrString(attrName);
	return Short::fromString(str);
}

uint16_t XmlElement::getAttrUShort(const char* attrName) const
		throw (NumberFormatException&)
{
	std::string str = getAttrString(attrName);
	return UShort::fromString(str);
}

int32_t XmlElement::getAttrInt(const char* attrName) const
		throw (NumberFormatException&)
{
	std::string str = getAttrString(attrName);
	return Integer::fromString(str);
}

uint32_t XmlElement::getAttrUInt(const char* attrName) const
		throw (NumberFormatException&)
{
	std::string str = getAttrString(attrName);
	return UInteger::fromString(str);
}

int64_t XmlElement::getAttrLong(const char* attrName) const
		throw (NumberFormatException&)
{
	std::string str = getAttrString(attrName);
	return Long::fromString(str);
}

uint64_t XmlElement::getAttrULong(const char* attrName) const
		throw (NumberFormatException&)
{
	std::string str = getAttrString(attrName);
	return ULong::fromString(str);
}

float XmlElement::getAttrFloat(const char* attrName) const
		throw (NumberFormatException&)
{
	std::string str = getAttrString(attrName);
	return Float::fromString(str);
}

std::string XmlElement::getAttrString(const char* attrName) const
{
	MX_ASSERT(!isNull());
	char* value = (char*) xmlGetProp(nodePtr, (const xmlChar*) attrName);

	if (NULL == value)
	{
		return "";
	}

	std::string ret(value);
	xmlFree(value);
	return ret;
}

XmlElement& XmlElement::setAttrShort(const char* attrName, int16_t value)
		throw (XmlException&)
{
	return setAttrString(attrName, Short(value).toString());
}

XmlElement& XmlElement::setAttrUShort(const char* attrName, uint16_t value)
		throw (XmlException&)
{
	return setAttrString(attrName, UShort(value).toString());
}

XmlElement& XmlElement::setAttrInt(const char* attrName, int32_t value)
		throw (XmlException&)
{
	return setAttrString(attrName, Integer(value).toString());
}

XmlElement& XmlElement::setAttrUInt(const char* attrName, uint32_t value)
		throw (XmlException&)
{
	return setAttrString(attrName, UInteger(value).toString());
}

XmlElement& XmlElement::setAttrLong(const char* attrName, int64_t value)
		throw (XmlException&)
{
	return setAttrString(attrName, Long(value).toString());
}

XmlElement& XmlElement::setAttrULong(const char* attrName, uint64_t value)
		throw (XmlException&)
{
	return setAttrString(attrName, ULong(value).toString());
}

XmlElement& XmlElement::setAttrFloat(const char* attrName, float value,
		int prec) throw (XmlException&)
{
	return setAttrString(attrName, Float(value).toString());
}

XmlElement& XmlElement::setAttrString(const char* attrName, const char* str)
		throw (XmlException&)
{
	MX_ASSERT(!isNull());
	MX_ASSERT(NULL != attrName && '\0' != *attrName);

	if (NULL == xmlSetProp(nodePtr, (const xmlChar*) attrName,
			(const xmlChar*) str))
	{
		THROW2(XmlException, std::string("Set attribute: [") + attrName + "] failed");
	}

	return *this;
}

XmlElement& XmlElement::setAttrString(const char* attrName,
		const std::string& str) throw (XmlException&)
{
	return setAttrString(attrName, str.c_str());
}

XmlElement XmlElement::getFirstChild(void) const
{
	MX_ASSERT(!isNull());

	XmlNode node = firstChild();

	while (!node.isNull())
	{
		if (node.isElement())
		{
			return XmlElement((xmlElementPtr) node.getNodePtr());
		}

		node = node.nextSibling();
	}

	return XmlElement();
}

XmlElement XmlElement::getLastChild(void) const
{
	MX_ASSERT(!isNull());

	XmlNode node = lastChild();

	while (!node.isNull())
	{
		if (node.isElement())
		{
			return XmlElement((xmlElementPtr) node.getNodePtr());
		}

		node = node.prevSibling();
	}

	return XmlElement();
}

XmlElement XmlElement::getNextSibling(void) const
{
	MX_ASSERT(!isNull());

	XmlNode node = nextSibling();

	while (!node.isNull())
	{
		if (node.isElement())
		{
			return XmlElement((xmlElementPtr) node.getNodePtr());
		}

		node = node.nextSibling();
	}

	return XmlElement();
}

XmlElement XmlElement::getPrevSibling(void) const
{
	MX_ASSERT(!isNull());

	XmlNode node = prevSibling();

	while (!node.isNull())
	{
		if (node.isElement())
		{
			return XmlElement((xmlElementPtr) node.getNodePtr());
		}

		node = node.prevSibling();
	}

	return XmlElement();
}

XmlElement XmlElement::getFirstChildByName(const char* name) const
{
	MX_ASSERT(!isNull());

	if ((NULL == name) || ('\0' == *name))
	{
		return XmlElement();
	}

	XmlElement elt = getFirstChild();

	while (!elt.isNull())
	{
		const char* nodeName = elt.getName();

		if (StringUtil::compare(nodeName, name) == 0)
		{
			return elt;
		}

		elt = elt.getNextSibling();
	}

	return XmlElement();
}

XmlElement XmlElement::getNextSiblingByName(const char* name) const
{
	MX_ASSERT(!isNull());

	if ((NULL == name) || ('\0' == *name))
	{
		return XmlElement();
	}

	XmlElement elt = getNextSibling();

	while (!elt.isNull())
	{
		const char* nodeName = elt.getName();

		if (StringUtil::compare(nodeName, name) == 0)
		{
			return elt;
		}

		elt = elt.getNextSibling();
	}

	return XmlElement();
}

XmlElement XmlElement::addChild(XmlElement child)
{
	MX_ASSERT(!isNull());

	if (child.isNull())
	{
		return child;
	}

	if (NULL == xmlAddChild(getNodePtr(), child.getNodePtr()))
	{
		return XmlElement();
	}

	return child;
}

XmlElement XmlElement::appendChild(XmlElement child)
{
	MX_ASSERT(!isNull());

	if (child.isNull())
	{
		return child;
	}

	XmlElement lastChild = getLastChild();

	if (lastChild.isNull())
	{
		return addChild(child);
	}

	if (NULL == xmlAddNextSibling(lastChild.getNodePtr(), child.getNodePtr()))
	{
		return XmlElement();
	}

	return child;
}

XmlElement XmlElement::appendChild(const char* name)
{
	MX_ASSERT(!isNull());

	XmlElement child = getDoc().createElement(name, NULL);

	if (!child.isNull())
	{
		XmlElement tmp = appendChild(child);

		if (tmp.isNull())
		{
			child.remove();
		}

		return tmp;
	}

	return child;
}

void XmlElement::removeChildSelf(void)
{
	MX_ASSERT(!isNull());
	xmlUnlinkNode(getNodePtr());
}

void XmlElement::removeChild(XmlElement elt)
{
	MX_ASSERT(!isNull());
	MX_ASSERT(!elt.isNull());
	xmlUnlinkNode(elt.getNodePtr());
}

} // namespace mxcore
