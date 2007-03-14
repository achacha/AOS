
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
  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    (*cit)->emitJSON(target, -1);
    ++cit;
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
    target.append(AString::sstr_TwoSpaces);
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
    return this;
  }
}

const AXmlNode *AXmlNode::findNode(const AString& xpath) const
{
  LIST_AString xparts;
  xpath.split(xparts, '/');

  if (xparts.size() > 0)
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
  pNode->setParent(this);
  m_Content.push_back(pNode);
}

AXmlNode& AXmlNode::addComment(const AString& comment)
{
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
