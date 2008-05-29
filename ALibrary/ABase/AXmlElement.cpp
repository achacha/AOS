/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AXmlElement.hpp"
#include "AString.hpp"
#include "ARope.hpp"
#include "AXmlInstruction.hpp"
#include "AXmlElement.hpp"
#include "AXmlData.hpp"
#include "AFile.hpp"

void AXmlElement::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AXmlElement @ " << std::hex << this << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Parent=" << std::hex << mp_Parent << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Name=" << m_Name << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Attributes={" << std::endl;
  m_Attributes.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Content={" << std::endl;
  CONTAINER::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    (*cit)->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AXmlElement::AXmlElement(AXmlElement *pParent) :
  mp_Parent(pParent)
{
}

AXmlElement::AXmlElement(const AString& name, AXmlElement *pParent) :
  mp_Parent(pParent),
  m_Name(name)
{
}

AXmlElement::AXmlElement(const AString& name, const AXmlAttributes& attrs, AXmlElement *pParent) :
  mp_Parent(pParent),
  m_Name(name),
  m_Attributes(attrs)
{
}

AXmlElement::AXmlElement(const AXmlElement& that, AXmlElement *pParent /* = NULL */) :
  mp_Parent(pParent),
  m_Name(that.m_Name),
  m_Attributes(that.m_Attributes)
{
  //a_By default parent node is not copied
  for (CONTAINER::const_iterator cit = that.m_Content.begin(); cit != that.m_Content.end(); ++cit)
    m_Content.push_back((*cit)->clone());
}

AXmlElement::~AXmlElement()
{
  try
  {
    for (CONTAINER::iterator it = m_Content.begin(); it != m_Content.end(); ++it)
      delete (*it);
  }
  catch(...) {}
}

void AXmlElement::clear()
{
  CONTAINER::iterator it = m_Content.begin();
  while (it != m_Content.end())
  {
    delete (*it);
    ++it;
  }
  m_Content.clear();
  m_Attributes.clear();
  mp_Parent = NULL;
}

bool AXmlElement::emitXmlFromPath(
  const AString& xpath, 
  AOutputBuffer& target,
  int indent //= -1
) const
{
  const AXmlElement *pNode = findElement(xpath);
  if (pNode)
  {
    AXmlElement::CONTAINER::const_iterator cit = pNode->m_Content.begin();
    while (cit != pNode->m_Content.end())
    {
      (*cit)->emit(target, indent);
      ++cit;
    }
    return true;
  }
  else
    return false;
}

bool AXmlElement::emitContentFromPath(
  const AString& xpath, 
  AOutputBuffer& target
) const
{
  const AXmlElement *pNode = findElement(xpath);
  if (pNode)
  {
    AXmlElement::CONTAINER::const_iterator cit = pNode->m_Content.begin();
    while (cit != pNode->m_Content.end())
    {
      (*cit)->emitContent(target);
      ++cit;
    }
    return true;
  }
  else
    return false;
}

AXmlElement& AXmlElement::emitXml(AXmlElement& thisRoot) const
{
  //TODO: This has to attach itself to thisRoot and setName instead?
  AXmlElement& base = thisRoot.addElement(m_Name);
  CONTAINER::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    base.addContent((*cit)->clone());
    ++cit;
  }
  return thisRoot;
}

void AXmlElement::emitXmlContent(AXmlElement& target) const
{
  CONTAINER::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    target.addContent((*cit)->clone());
    ++cit;
  }
}

void AXmlElement::emitContent(AOutputBuffer& target) const
{
  CONTAINER::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {   
    (*cit)->emitContent(target);
    ++cit;
  }
}

void AXmlElement::_indent(AOutputBuffer& target, int indent) const
{
  while (indent > 0)
  {
    target.append(AConstant::ASTRING_TWOSPACES);
    --indent;
  }
}

AXmlElement& AXmlElement::addAttribute(const AString& name, const AString& value)
{
  m_Attributes.insert(name, value);

  return *this;
}

AXmlElement *AXmlElement::findElement(const AString& xpath)
{
  LIST_AString xparts;
  xpath.split(xparts, '/');

  if (xparts.size() > 0)
  {
    if ('/' == xpath.at(0))
    {
      if (m_Name.equals(xparts.front()))
      {
        xparts.pop_front();
        if (xparts.size() > 0)
          return _get(xparts);
        else
          return this;
      }
      else
        return NULL;
    }
    else
    {
      if (xparts.size() > 0)
        return _get(xparts);
      else
        return NULL;
    }
  }
  else
  {
    return this;
  }
}

const AXmlElement *AXmlElement::findElement(const AString& xpath) const
{
  LIST_AString xparts;
  xpath.split(xparts, '/');

  if (xparts.size() > 0)
  {
    if ('/' == xpath.at(0))
    {
      if (m_Name.equals(xparts.front()))
      {
        xparts.pop_front();
        if (xparts.size() > 0)
          return _get(xparts);
        else
          return this;
      }
      else
        return NULL;
    }
    else
    {
      if (xparts.size() > 0)
        return _get(xparts);
      else
        return NULL;
    }
  }
  else
  {
    return this;
  }
}

AXmlElement *AXmlElement::_get(LIST_AString& xparts) const
{
  AString& strName = xparts.front();
  CONTAINER::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    if ((*cit)->getName().equals(strName))
    {
      xparts.pop_front();
      if (xparts.size() > 0)
        return (*cit)->_get(xparts);
      else 
        return (*cit);
    }
    ++cit;
  }
  
  //a_Not found
  return NULL;
}

AXmlElement& AXmlElement::addComment(const AString& comment)
{
  AASSERT(this, m_Content.size() < DEBUG_MAXSIZE_AXmlElement);  //Debug only limit

  AXmlInstruction *p = new AXmlInstruction(AXmlInstruction::COMMENT, this);
  p->useData().assign(comment);
  m_Content.push_back(p);

  return *this;
}

const AString& AXmlElement::getName() const 
{ 
  return m_Name;
}

AString& AXmlElement::useName() 
{ 
  return m_Name; 
}

const AXmlAttributes& AXmlElement::getAttributes() const
{
  return m_Attributes; 
}

AXmlAttributes& AXmlElement::useAttributes() 
{ 
  return m_Attributes; 
}

const AXmlElement::CONTAINER& AXmlElement::getContentContainer() const 
{
  return m_Content;
}

size_t AXmlElement::find(const AString& path, AXmlElement::CONST_CONTAINER& result) const
{
  AString strAttribute;
  AString strPath(path);
  bool leadingSlash = false;

  size_t pos = path.find('@');
  if (AConstant::npos != pos)
  {
    //a_Extract attribute
    strPath.get(strAttribute, pos);
    strPath.setSize(strPath.getSize() - 1);  //a_Remove the trailing '@'
  }

  if ('/' == strPath.at(0))
  {
    leadingSlash = true;  //a_Signals that slash leads the path so much include current name
  }

  //a_Remove the leading name (which should be this element's name)
  LIST_AString listPath;
  strPath.split(listPath, '/');
  if (leadingSlash)
  {
    if (0 == listPath.size())
    {
      //a_ "/" selects current node
      result.push_back(this);
      return 1;
    }
    else if (0 != listPath.front().compare(m_Name))
    {
      //a_First token MUST be the name of this element if / leads
      ATHROW_EX(this, AException::ProgrammingError, ARope("Absolute path must start with the name of the current root element: root=/")+m_Name+" while path="+path);
    }
  }
  else if (0 == listPath.size())
  {
    //a_ "/" selects current node
    result.push_back(this);
    return 1;
  }
  else
  {
    //a_Relative path implies this node is first
    listPath.push_front(m_Name);
  }

  return _const_find(listPath, result);
}

size_t AXmlElement::_find(const AString& path, AXmlElement::CONTAINER& result)
{
  AString strAttribute;
  AString strPath(path);
  bool leadingSlash = false;

  size_t pos = path.find('@');
  if (AConstant::npos != pos)
  {
    //a_Extract attribute
    strPath.get(strAttribute, pos);
    strPath.setSize(strPath.getSize() - 1);  //a_Remove the trailing '@'
  }

  if ('/' == strPath.at(0))
  {
    leadingSlash = true;  //a_Signals that slash leads the path so much include current name
  }

  //a_Remove the leading name (which should be this element's name)
  LIST_AString listPath;
  strPath.split(listPath, '/');
  if (leadingSlash)
  {
    if (0 == listPath.size())
    {
      //a_ "/" selects current node
      result.push_back(this);
      return 1;
    }
    else if (0 != listPath.front().compare(m_Name))
    {
      //a_First token MUST be the name of this element if / leads
      ATHROW_EX(this, AException::ProgrammingError, ARope("Absolute path must start with the name of the current root element: root=/")+m_Name+" while path="+path);
    }
  }
  else if (0 == listPath.size())
  {
    //a_ "/" selects current node
    result.push_back(this);
    return 1;
  }
  else
  {
    //a_Relative path implies this node is first
    listPath.push_front(m_Name);
  }

  return _nonconst_find(listPath, result);
}

size_t AXmlElement::_const_find(LIST_AString listPath, AXmlElement::CONST_CONTAINER& result) const
{
  if (listPath.front().equals(m_Name))
  {
    listPath.pop_front();

    size_t ret = 0;
    switch(listPath.size())
    {
      case 0:
        //a_This node is it
        result.push_back(this);
        ++ret;
      break;
      
      case 1:
      {
        //a_Include immediate children nodes
        CONTAINER::const_iterator cit = m_Content.begin();
        while (cit != m_Content.end())
        {
          if ((*cit)->getName().equals(listPath.front()))
          {
            result.push_back(*cit);
            ++ret;
          }
          ++cit;
        }
      }
      break;
      
      default:
      {
        //a_Recurse deeper for each element
        CONTAINER::const_iterator cit = m_Content.begin();
        while (cit != m_Content.end())
        {
          if ((*cit)->getName().equals(listPath.front()))
          {
            ret += (*cit)->_const_find(listPath, result);
          }
          ++cit;
        }
      }
      break;
    }

    return ret;
  }
  else
    return 0;
}

size_t AXmlElement::_nonconst_find(LIST_AString listPath, AXmlElement::CONTAINER& result)
{
  if (listPath.front().equals(m_Name))
  {
    listPath.pop_front();

    size_t ret = 0;
    switch(listPath.size())
    {
      case 0:
        //a_This node is it
        result.push_back(this);
        ++ret;
      break;
      
      case 1:
      {
        //a_Include immediate children nodes
        CONTAINER::const_iterator cit = m_Content.begin();
        while (cit != m_Content.end())
        {
          if ((*cit)->getName().equals(listPath.front()))
          {
            result.push_back(*cit);
            ++ret;
          }
          ++cit;
        }
      }
      break;
      
      default:
      {
        //a_Recurse deeper for each element
        CONTAINER::const_iterator cit = m_Content.begin();
        while (cit != m_Content.end())
        {
          if ((*cit)->getName().equals(listPath.front()))
          {
            ret += (*cit)->_nonconst_find(listPath, result);
          }
          ++cit;
        }
      }
      break;
    }

    return ret;
  }
  else
    return 0;
}

bool AXmlElement::exists(const AString& path) const
{
  return (NULL != findElement(path));
}

bool AXmlElement::emitString(const AString& path, AOutputBuffer& target) const
{
  return emitContentFromPath(path, target);
}

AString AXmlElement::getString(const AString& path, const AString& strDefault) const
{
  AString str;
  if (emitContentFromPath(path, str))
    return str;
  else
    return strDefault;
}          

int AXmlElement::getInt(const AString& path, int iDefault) const
{
  AString str;
  if (emitContentFromPath(path, str))
    return str.toInt();
  else
    return iDefault;
}

u4 AXmlElement::getU4(const AString& path, u4 u4Default) const
{
  AString str;
  if (emitContentFromPath(path, str))
    return str.toU4();
  else
    return u4Default;
}

u8 AXmlElement::getU8(const AString& path, u8 u8Default) const
{
  AString str;
  if (emitContentFromPath(path, str))
    return str.toU8();
  else
    return u8Default;
}

size_t AXmlElement::getSize_t(const AString& path, size_t iDefault) const
{
  AString str;
  if (emitContentFromPath(path, str))
    return str.toSize_t();
  else
    return iDefault;
}

bool AXmlElement::getBool(const AString& path, bool boolDefault) const
{
  AString str;
  if (emitContentFromPath(path, str))
  {
    return str.equalsNoCase(AConstant::ASTRING_TRUE) || str.equals("1",1);
  }
  else
    return boolDefault;
}

bool AXmlElement::hasElements() const
{
  CONTAINER::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    //a_Stop when first instance is encountered
    if ((*cit)->isElement())
      return true;

    ++cit;
  }
  return false;
}

AXmlElement *AXmlElement::_getOrCreate(LIST_AString& xparts, AXmlElement* pParent)
{
  AString& strName = xparts.front();
  CONTAINER::iterator it = m_Content.begin();
  while (it != m_Content.end())
  {
    if ((*it)->getName().equals(strName))
    {
      AXmlElement *p = dynamic_cast<AXmlElement *>(*it);
      if (!xparts.size())
      {
        return p;
      }
      else
      {
        if (p)
        {
          xparts.pop_front();
          if (!xparts.size())
            return p;
          else
            return p->_getOrCreate(xparts, this);
        }
        else
          ATHROW_EX(this, AException::DataConflict, AString("Not AXmlElement type at: ")+strName);
      }
    }
    ++it;
  }
  
  //a_strName not found, create it
  AXmlElement *p = new AXmlElement(strName, pParent);
  addContent(p);
  xparts.pop_front();
  if (xparts.size() == 0)
  {
    return p;
  }
  else
  {
    return p->_getOrCreate(xparts, this);
  }
}

AXmlElement *AXmlElement::_createAndAppend(LIST_AString& xparts, AXmlElement *pParent)
{
  AString& strName = xparts.front();
  AXmlElement *p = new AXmlElement(strName, pParent);
  addContent(p);
  xparts.pop_front();
  if (xparts.size() > 0)
    return p->_createAndAppend(xparts, this);
  else
    return p;
}

AXmlElement& AXmlElement::addElement(const AString& path)
{
  AXmlElement *p = _addElement(path, false);
  AASSERT(this, p);
  return *p;
}

AXmlElement& AXmlElement::overwriteElement(const AString& path)
{
  AXmlElement *p = _addElement(path, true);
  AASSERT(this, p);
  return *p;
}

AXmlElement& AXmlElement::addElement(const AString& path, const AEmittable& object, AXmlElement::Encoding encoding, bool overwrite)
{
  AXmlElement *p = _addElement(path, overwrite);
  AASSERT(this, p);

  //a_Emit to rope
  ARope value;
  object.emit(value);
  p->addData(value, encoding);

  return *p;
}

AXmlElement *AXmlElement::_addElement(const AString& path, bool overwrite)
{
  if (m_Name.isEmpty())
    ATHROW_EX(this, AException::InvalidObject, ASWNL("AXmlElement does not have a name"));

  LIST_AString xparts;
  path.split(xparts, '/');
  if (!xparts.size())
    ATHROW(this, AException::InvalidParameter);

  //a_Check is absolute is used and if root name matches this element
  if ('/' == path.at(0) && xparts.size() > 0)
  {
    //a_xpath starts with /, make sure names match
    if (xparts.front().equals(m_Name))
      xparts.pop_front();
    else
      ATHROW_EX(this, AException::InvalidPath, ARope("Path specified (")+path+ASWNL(") is absolute and does not match this element's name: ")+m_Name);
  }

  //a_Skipped over root or relative path specified
  AXmlElement *p = NULL;
  if (xparts.size() > 0)
  { 
    if (overwrite)
      p = _getOrCreate(xparts, this);
    else
      p = _createAndAppend(xparts, this);
  }
  else
    p = this;

  return p;
}

AXmlElement& AXmlElement::addData(const size_t value)
{
  _addData(AString::fromSize_t(value), AXmlElement::ENC_NONE);
  return *this;
}

AXmlElement& AXmlElement::addData(const AEmittable& data, AXmlElement::Encoding encoding)
{
  _addData(data, encoding);
  return *this;
}

AXmlElement& AXmlElement::addData(const double value)
{
  _addData(AString::fromDouble(value), AXmlElement::ENC_NONE);
  return *this;
}

AXmlElement& AXmlElement::addData(const int value)
{
  _addData(AString::fromInt(value), AXmlElement::ENC_NONE);
  return *this;
}

AXmlElement& AXmlElement::addData(const char value)
{
  _addData(ASW(&value,1), AXmlElement::ENC_NONE);
  return *this;
}

AXmlElement& AXmlElement::addData(
  const char *value, 
  size_t length,                 // = AConstant::npos
  AXmlElement::Encoding encoding // = AXmlElement::ENC_NONE
)
{
  _addData(AString::wrap(value, length), encoding);
  return *this;
}

AXmlElement& AXmlElement::setData(const size_t value)
{
  _addData(AString::fromSize_t(value), AXmlElement::ENC_NONE, true);
  return *this;
}

AXmlElement& AXmlElement::setData(const AEmittable& data, AXmlElement::Encoding encoding)
{
  _addData(data, encoding, true);
  return *this;
}

AXmlElement& AXmlElement::setData(const double value)
{
  _addData(AString::fromDouble(value), AXmlElement::ENC_NONE, true);
  return *this;
}

AXmlElement& AXmlElement::setData(const int value)
{
  _addData(AString::fromInt(value), AXmlElement::ENC_NONE, true);
  return *this;
}

AXmlElement& AXmlElement::setData(const char value)
{
  _addData(ASW(&value,1), AXmlElement::ENC_NONE, true);
  return *this;
}

AXmlElement& AXmlElement::setData(
  const char *value, 
  size_t length,                 // = AConstant::npos
  AXmlElement::Encoding encoding // = AXmlElement::ENC_NONE
)
{
  _addData(AString::wrap(value, length), encoding, true);
  return *this;
}

void AXmlElement::_addData(
  const AEmittable& value, 
  AXmlElement::Encoding encoding, 
  bool overwrite // = false
)
{
  AASSERT(this, m_Content.size() < DEBUG_MAXSIZE_AXmlElement);  //Debug only limit

  AXmlData *p = new AXmlData(value, encoding);
  p->setParent(this);
  
  //a_Overwrite clears existing content
  if (overwrite)
    m_Content.clear();

  m_Content.push_back(p);
}

AXmlElement& AXmlElement::addAttribute(const AString& name, const double value)
{
  return addAttribute(name, AString::fromDouble(value));
}

AXmlElement& AXmlElement::addAttribute(const AString& name, const u8 value)
{
  return addAttribute(name, AString::fromU8(value));
}

AXmlElement& AXmlElement::addAttribute(const AString& name, const u4 value)
{
  return addAttribute(name, AString::fromU4(value));
}

AXmlElement& AXmlElement::addAttributes(const AXmlAttributes& attrs)
{
  m_Attributes.append(attrs);

  return *this;
}

AXmlElement& AXmlElement::addContent(
  AXmlElement *pnode, 
  const AString& path // = AConstant::ASTRING_EMPTY
)
{
  if (path.isEmpty() || path.equals(AConstant::ASTRING_SLASH))
  {
    AASSERT(this, m_Content.size() < DEBUG_MAXSIZE_AXmlElement);  //Debug only limit
    AASSERT(this, pnode);

    pnode->setParent(this);
    m_Content.push_back(pnode);
  }
  else
  {
    LIST_AString parts;
    path.split(parts, '/');
    if (!parts.size())
      ATHROW(this, AException::InvalidParameter);

    AXmlElement *pNewParent = _createAndAppend(parts,this);
    AASSERT_EX(this, pNewParent, path);
    pNewParent->addContent(pnode);
  }
  return *this;
}

AXmlElement& AXmlElement::addContent(const AXmlEmittable& data)
{
  data.emitXml(*this);  
  return *this;
}

void AXmlElement::emit(AOutputBuffer& target) const
{
  emit(target, -1);
}

void AXmlElement::emit(AOutputBuffer& target, int indent) const
{
  if (!m_Name.isEmpty())
  {
    //a_Element has a name
    if (indent >= 0)
    {
      if (indent)
        target.append(AConstant::ASTRING_CRLF);                                    //a_End of line
      for (int i=0; i<indent; ++i) target.append(AConstant::ASTRING_TWOSPACES);    //a_Indentation
    }
    target.append(AXmlElement::sstr_Start);
    target.append(m_Name);

    //a_Display attributes of this element
    if (m_Attributes.size() > 0)
    {
      target.append(AConstant::ASTRING_SPACE);
      m_Attributes.emit(target);
    }

    CONTAINER::const_iterator cit = m_Content.begin();
    int iSubElementCount = 0;
    if (cit != m_Content.end())
    {
      target.append(AXmlElement::sstr_End);
      while (cit != m_Content.end())
      {
        if ((*cit)->isData())
        {
          (*cit)->emit(target, indent);
        }
        else
        {
          (*cit)->emit(target, (indent >= 0 ? indent+1 : indent));
          ++iSubElementCount;
        }
        ++cit;
      }
      if (iSubElementCount > 0 && indent >= 0)
      {
        target.append(AConstant::ASTRING_CRLF);                                      //a_End of line
        _indent(target, indent);
      }
      target.append(AXmlElement::sstr_StartEnd);
      target.append(m_Name);
      target.append(AXmlElement::sstr_End);
    }
    else
    {
      target.append(AXmlElement::sstr_EndSingular);
    }
  }
  else
  {
    //a_Element has no name, just iterate the children if any
    CONTAINER::const_iterator cit = m_Content.begin();
    if (cit != m_Content.end())
    {
      while (cit != m_Content.end())
      {
        (*cit)->emit(target, indent);
        ++cit;
      }
    }
  }
}

void AXmlElement::emitJson(
  AOutputBuffer& target,
  int indent // = -1
) const
{
  if (indent >=0) _indent(target, indent);
  target.append(m_Name);
  if (m_Content.size() + m_Attributes.size() == 0)
  {
    target.append(":''",4);
    if (indent >=0) target.append(AConstant::ASTRING_CRLF);
    return;
  }
  
  bool needsBraces = m_Content.size() > 0 || m_Attributes.size() > 0;
  target.append(':');
  if (needsBraces)
  {
    target.append('{');
    if (indent >=0) target.append(AConstant::ASTRING_CRLF);
  }

  //a_Add content
  size_t attrSize = m_Attributes.size();
  if (m_Content.size() > 0)
  {
    CONTAINER::const_iterator cit = m_Content.begin();
    while(cit != m_Content.end())
    {
      (*cit)->emitJson(target, (indent >= 0 ? indent+1 : indent));
      ++cit;
      if (cit != m_Content.end() || attrSize > 0)
      {
        target.append(',');
        if (indent >=0) target.append(AConstant::ASTRING_CRLF);
      }
    }
  }

  //a_Attributes become name:value pairs
  if (attrSize > 0)
  {
    SET_AString names;
    m_Attributes.getNames(names);
    AXmlAttributes::CONTAINER::const_iterator cit = m_Attributes.getAttributeContainer().begin();
    const AXmlAttributes::CONTAINER& container = m_Attributes.getAttributeContainer();
    while(cit != container.end())
    {
      if (indent >=0) _indent(target, indent+1);
      target.append((*cit)->getName());
      target.append(":'",2);
      target.append((*cit)->getValue());
      target.append('\'');
      ++cit;
      if (cit != container.end())
      {
        target.append(',');
        if (indent >= 0) target.append(AConstant::ASTRING_CRLF);
      }
    }
  }

  if (needsBraces)
  {
    
    if (indent >=0)
    {
      target.append(AConstant::ASTRING_CRLF);
      _indent(target, indent);
    }
    target.append('}');
  }
}

void AXmlElement::fromAFile(AFile& file)
{
  AString str(1024, 256);

  //a_Find <
  if (AConstant::npos == file.skipUntilOneOf('<'))
    return;

  //a_Skip over whitespace between < and tagname
  file.skipOver();

  //a_Extract name and skip over whitespace
  m_Name.clear();
  file.readUntilOneOf(m_Name, AXmlElement::sstr_EndOrWhitespace, false);
  file.skipOver();
  
  //a_Find /> or >
  char c = ' ';
  if (0 == m_Name.find(ASW("!--",3)))
  {
    if (m_Name.rfind("--", 2) == m_Name.getSize() - 2)  //TODO:
    //a_Special case for comment, must end with '-->' and may contain '>' inside
    file.readUntil(str, AXmlElement::sstr_EndComment);
    addComment(str);
    return;
  }
  else
    file.readUntilOneOf(str, AXmlElement::sstr_End);

  if (!str.isEmpty() && str.at(str.getSize() - 1) == '/')
  {
    c = '/';  //a_Singular mode
    str.setSize(str.getSize() - 1);
  }
  str.stripTrailing();

  //a_Parse attributes
  if (!str.isEmpty())
    m_Attributes.parse(str);

  bool boolEndTagFound = (c == '/');
  while (!boolEndTagFound)
  {
    //a_Read data until next tag starts
    file.skipOver();                //a_Skip over whitespace
    str.clear();
    if (AConstant::npos == file.readUntilOneOf(str, AXmlElement::sstr_Start))
      break;

    str.stripTrailing();
    if (!str.isEmpty())
    {
      addData(str);
    }

    //a_Skip over whitespace between < and start of the tag name
    file.skipOver();

    file.peek(c);
    switch(c)
    {
      case '/':
      {
        //a_End of tag found
        file.read(c);
        str.clear();
        file.readUntilOneOf(str, AXmlElement::sstr_EndOrWhitespace);
        file.skipOver(AXmlElement::sstr_EndOrWhitespace);                //a_Skip over whitespace and >
        if (str != m_Name)
          ATHROW_EX(&file, AException::InvalidData, AString("Close tag </")+str+"> does not match opened tag <"+m_Name+">");
        
        boolEndTagFound = true;
      }
      break;

      //a_Handle embedded CDATA, comments and such
      case '!':
      {
        file.putBack('<');
        str.clear();
        
        file.peek(str, 9);    //a_ either "<!--*--"  or "<![CDATA["
        if (0 == str.findNoCase(AXmlData::sstr_StartCDATA))
        {
          //a_CDATA found
          file.skip(AXmlData::sstr_StartCDATA.getSize());

          AString data;
          file.readUntil(data, AXmlData::sstr_EndCDATA, true, true);
          addData(data, AXmlElement::ENC_CDATADIRECT);
        }
        else if (0 == str.findNoCase(AXmlElement::sstr_StartComment))
        {
          //a_Comment (!--) found
          file.skip(AXmlElement::sstr_StartComment.getSize());

          AString comment;
          file.readUntil(comment, AXmlElement::sstr_EndComment, true, true);
          addComment(comment);
        }
        else
        {
          addData(str);
        }
      }
      break;

      default:
      {
        //a_Put the start tag back
        file.putBack('<');

        //a_Another element
        AAutoPtr<AXmlElement> psubElement(new AXmlElement(), true);
        psubElement->fromAFile(file);
        psubElement.setOwnership(false);
        addContent(psubElement.use());
      }
      break;
    }
  }
}

void AXmlElement::toAFile(AFile& file) const
{
  ARope rope;
  emit(rope);
  rope.write(file);
}

void AXmlElement::setString(const AString& path, const AString& value, AXmlElement::Encoding encoding)
{
  addElement(path, value, encoding, true);
}

void AXmlElement::setInt(const AString& path, int value)
{
  _addElement(path, true)->setData(value);
}

void AXmlElement::setSize_t(const AString& path, size_t value)
{
  _addElement(path, true)->setData(value);
}

void AXmlElement::setBool(const AString& path, bool value)
{
  _addElement(path, true)->setData(value ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE);
}

bool AXmlElement::isElement() const
{
  return true;
}

bool AXmlElement::isData() const
{
  return false;
}

bool AXmlElement::isInstruction() const
{
  return false;
}

AXmlElement* AXmlElement::clone() const 
{ 
  return new AXmlElement(*this);
}

bool AXmlElement::remove(const AString& path)
{
  AXmlElement::CONTAINER result;
  if (_find(path, result) > 0)
  {
    for (AXmlElement::CONTAINER::iterator it = result.begin(); result.end() != it; ++it)
    {
      AXmlElement *p = (*it)->getParent();
      p->_removeChildElement(*it);
    }
    return true;
  }
  else
    return false;
}

void AXmlElement::_removeChildElement(AXmlElement *pElement)
{
  for (AXmlElement::CONTAINER::iterator it = m_Content.begin(); m_Content.end() != it; ++it)
  {
    if (pElement == *it)
    {
      m_Content.erase(it);
      delete pElement;
      return;
    }
  }
}
