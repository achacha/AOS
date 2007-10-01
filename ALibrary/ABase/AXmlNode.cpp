
#include "pchABase.hpp"

#include "AXmlNode.hpp"
#include "AString.hpp"
#include "ARope.hpp"
#include "AXmlInstruction.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
void AXmlNode::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AXmlNode @ " << std::hex << this << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Parent=" << std::hex << mp_Parent << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Name=" << m_Name << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Attributes {" << std::endl;
  m_Attributes.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AXmlNode::AXmlNode(AXmlNode *pParent) :
  mp_Parent(pParent)
{
}

AXmlNode::AXmlNode(const AString& name, AXmlNode *pParent) :
  mp_Parent(pParent),
  m_Name(name)
{
}

AXmlNode::AXmlNode(const AAttributes& attrs, AXmlNode *pParent) :
  mp_Parent(pParent),
  m_Attributes(attrs)
{
}

AXmlNode::AXmlNode(const AString& name, const AAttributes& attrs, AXmlNode *pParent) :
  mp_Parent(pParent),
  m_Name(name),
  m_Attributes(attrs)
{
}

AXmlNode::~AXmlNode()
{
  NodeContainer::iterator it = m_Content.begin();
  while (it != m_Content.end())
  {
    delete (*it);
    ++it;
  }
}

void AXmlNode::clear()
{
  NodeContainer::iterator it = m_Content.begin();
  while (it != m_Content.end())
  {
    delete (*it);
    ++it;
  }
  m_Content.clear();
  m_Attributes.clear();
  mp_Parent = NULL;
}

void AXmlNode::emit(AOutputBuffer& target) const
{
  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    (*cit)->emit(target, -1);
    ++cit;
  }
}

void AXmlNode::emit(AOutputBuffer& target, int indent) const
{
  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    (*cit)->emit(target, indent);
    ++cit;
  }
}

bool AXmlNode::emitFromPath(
  const AString& xpath, 
  AOutputBuffer& target,
  int indent //= -1
) const
{
  const AXmlNode *pNode = findNode(xpath);
  if (pNode)
  {
    AXmlNode::NodeContainer::const_iterator cit = pNode->m_Content.begin();
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

void AXmlNode::emitJSON(
  AOutputBuffer& target,
  int indent // = -1
) const
{
  bool needsBraces = (m_Content.size() > 0 || m_Attributes.size() > 0);
  if (needsBraces)
  {
    target.append('{');
  }

  //a_Attributes become name:value pairs
  if (m_Attributes.size() > 0)
  {
    SET_AString names;
    m_Attributes.getNames(names);
    for(LIST_NVPair::const_iterator cit = m_Attributes.getAttributeContainer().begin(); cit != m_Attributes.getAttributeContainer().end(); ++cit)
    {
      if (indent >=0) _indent(target, indent+1);
      target.append(cit->getName());
      target.append(':');
      target.append(cit->getValue());
      if (indent >=0) target.append(AConstant::ASTRING_CRLF);
    }
  }

  //a_Add content
  if (m_Content.size() > 0)
  {
    if (indent >=0) _indent(target, indent+1);
    target.append(m_Name);
    target.append(':');
    for (NodeContainer::const_iterator cit = m_Content.begin(); cit != m_Content.end(); ++cit)
    {
      (*cit)->emitJSON(target, (indent >= 0 ? indent+1 : indent));
    }
  }

  if (needsBraces)
  {
    if (indent >=0) _indent(target, indent);
    target.append('}');
    if (indent >=0) target.append(AConstant::ASTRING_CRLF);
  }
}

void AXmlNode::emit(AXmlElement& target) const
{
  AXmlElement& base = target.addElement(m_Name);
  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    base.addContent((*cit)->clone());
    ++cit;
  }
}

void AXmlNode::emitContent(AOutputBuffer& target) const
{
  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {   
    (*cit)->emitContent(target);
    ++cit;
  }
}

void AXmlNode::_indent(AOutputBuffer& target, int indent) const
{
  while (indent > 0)
  {
    target.append(AConstant::ASTRING_TWOSPACES);
    --indent;
  }
}

AXmlNode& AXmlNode::addAttribute(const AString& name, const AString& value)
{
  m_Attributes.insert(name, value);

  return *this;
}

AXmlNode& AXmlNode::addAttributes(const AAttributes& attrs)
{
  m_Attributes.append(attrs);

  return *this;
}

AXmlNode *AXmlNode::findNode(const AString& xpath)
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

const AXmlNode *AXmlNode::findNode(const AString& xpath) const
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

AXmlNode *AXmlNode::_get(LIST_AString& xparts) const
{
  AString& strName = xparts.front();
  NodeContainer::const_iterator cit = m_Content.begin();
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

void AXmlNode::addContentNode(AXmlNode *pNode)
{
  AASSERT(this, m_Content.size() < DEBUG_MAXSIZE_AXmlNode);  //Debug only limit
  AASSERT(this, pNode);

  pNode->setParent(this);
  m_Content.push_back(pNode);
}

AXmlNode& AXmlNode::addComment(const AString& comment)
{
  AASSERT(this, m_Content.size() < DEBUG_MAXSIZE_AXmlNode);  //Debug only limit

  AXmlInstruction *p = new AXmlInstruction(AXmlInstruction::COMMENT, this);
  p->useData().assign(comment);
  m_Content.push_back(p);

  return *this;
}

const AString& AXmlNode::getName() const 
{ 
  return m_Name;
}

AString& AXmlNode::useName() 
{ 
  return m_Name; 
}

const AAttributes& AXmlNode::getAttributes() const
{
  return m_Attributes; 
}

AAttributes& AXmlNode::useAttributes() 
{ 
  return m_Attributes; 
}

const AXmlNode::NodeContainer& AXmlNode::getContentContainer() const 
{
  return m_Content;
}

size_t AXmlNode::find(const AString& path, AXmlNode::ConstNodeContainer& result) const
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

  return _find(listPath, result);
}

size_t AXmlNode::_find(LIST_AString listPath, AXmlNode::ConstNodeContainer& result) const
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
        NodeContainer::const_iterator cit = m_Content.begin();
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
        NodeContainer::const_iterator cit = m_Content.begin();
        while (cit != m_Content.end())
        {
          if ((*cit)->getName().equals(listPath.front()))
          {
            ret += (*cit)->_find(listPath, result);
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


bool AXmlNode::exists(const AString& path) const
{
  return (NULL != findNode(path));
}

bool AXmlNode::emitString(const AString& path, AOutputBuffer& target) const
{
  return emitFromPath(path, target);
}

AString AXmlNode::getString(const AString& path, const AString& strDefault) const
{
  AString str;
  if (emitFromPath(path, str))
    return str;
  else
    return strDefault;
}          

int AXmlNode::getInt(const AString& path, int iDefault) const
{
  AString str;
  if (emitFromPath(path, str))
    return str.toInt();
  else
    return iDefault;
}

size_t AXmlNode::getSize_t(const AString& path, size_t iDefault) const
{
  AString str;
  if (emitFromPath(path, str))
    return str.toSize_t();
  else
    return iDefault;
}

bool AXmlNode::getBool(const AString& path, bool boolDefault) const
{
  AString str;
  if (emitFromPath(path, str))
  {
    return str.equalsNoCase("true",4) || str.equals("1",1);
  }
  else
    return boolDefault;
}

bool AXmlNode::hasElements() const
{
  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    //a_Stop when first instance is encountered
    if ((*cit)->isElement())
      return true;

    ++cit;
  }
  return false;
}

