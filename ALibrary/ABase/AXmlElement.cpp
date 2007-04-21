
#include "pchABase.hpp"

#include "AXmlElement.hpp"
#include "ARope.hpp"
#include "AFile.hpp"
#include "AXmlDocument.hpp"
#include "AObjectBase.hpp"
#include "AEmittable.hpp"
#include "AXmlEmittable.hpp"

#ifdef __DEBUG_DUMP__
void AXmlElement::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AXmlElement @ " << std::hex << this << std::dec << ") {" << std::endl;
  AXmlNode::debugDump(os, indent+1);

  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    (*cit)->debugDump(os, indent+1);
    ++cit;
  }

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AXmlElement::AXmlElement(AXmlNode *pParent /* = NULL */) :
  AXmlNode(pParent)
{
}

AXmlElement::AXmlElement(const AString& name, AXmlNode *pParent /* = NULL */) :
  AXmlNode(name, pParent)
{
}

AXmlElement::AXmlElement(const AString& name, const AAttributes& attrs, AXmlNode *pParent) :
  AXmlNode(name, attrs, pParent)
{
}

AXmlElement::AXmlElement(const AXmlElement& that, AXmlNode *pParent /* = NULL */) :
  AXmlNode(that.m_Name, that.m_Attributes, pParent)
{
  //a_By default parent node is not copied
  NodeContainer::const_iterator cit = that.m_Content.begin();
  while (cit != that.m_Content.end())
  {
    AXmlNode *p = (*cit)->clone();
    m_Content.push_back(p);
    ++cit;
  }
}

AXmlElement::~AXmlElement()
{
}

void AXmlElement::clear()
{
  AXmlNode::clear();
}

AXmlElement *AXmlElement::_getAndInsert(LIST_AString& xparts, AXmlNode* pParent)
{
  AString& strName = xparts.front();
  NodeContainer::iterator it = m_Content.begin();
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
            return p->_getAndInsert(xparts, this);
        }
        else
          ATHROW_EX(this, AException::DataConflict, AString("Not AXmlElement type at: ")+strName);
      }
    }
    ++it;
  }
  
  //a_strName not found, create it
  AXmlElement *p = new AXmlElement(strName);
  addContent(p);
  xparts.pop_front();
  if (xparts.size() == 0)
  {
    return p;
  }
  else
  {
    return p->_getAndInsert(xparts, this);
  }
}

AXmlElement *AXmlElement::_createAndAppend(LIST_AString& xparts, AXmlNode *pParent)
{
  AString& strName = xparts.front();
  AXmlElement *p = new AXmlElement(strName, this);
  addContent(p);
  xparts.pop_front();
  if (xparts.size() > 0)
    return p->_createAndAppend(xparts, this);
  else
    return p;
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const AString& value, AXmlData::Encoding encoding, bool insert)
{
  LIST_AString xparts;
  xpath.split(xparts, '/');
  if (!xparts.size())
    ATHROW(this, AException::InvalidParameter);

  AString strName(xparts.back());
  xparts.pop_back();
  AXmlElement *pParent = NULL;
  if (xparts.size() > 0)
  { 
    if (insert)
      pParent = _getAndInsert(xparts, this);
    else
      pParent = _createAndAppend(xparts, this);
  }
  else
    pParent = this;

  AXmlElement *pNew = new AXmlElement(strName);
  pNew->addData(value, encoding);

  pParent->addContent(pNew);
  return *pNew;
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const ARope& value, AXmlData::Encoding encoding, bool insert)
{
  if (m_Name.isEmpty())
    ATHROW_EX(this, AException::InvalidObject, "AXmlElement does not have a name");

  LIST_AString xparts;
  xpath.split(xparts, '/');
  if (!xparts.size())
    ATHROW(this, AException::InvalidParameter);

  AXmlElement *p = NULL;
  if (insert)
    p = _getAndInsert(xparts, this);
  else
    p = _createAndAppend(xparts, this);

  p->addData(value, encoding);
  return *p;
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const AEmittable& object, AXmlData::Encoding encoding, bool insert)
{
  if (m_Name.isEmpty())
    ATHROW_EX(this, AException::InvalidObject, "AXmlElement does not have a name");

  LIST_AString xparts;
  xpath.split(xparts, '/');
  if (!xparts.size())
    ATHROW(this, AException::InvalidParameter);

  //a_Emit to rope
  ARope value;
  object.emit(value);

  //a_Add rope as data
  AXmlElement *p = NULL;
  if (insert)
    p = _getAndInsert(xparts, this);
  else
    p = _createAndAppend(xparts, this);

  p->addData(value, encoding);
  return *p;
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const AXmlEmittable& object, bool insert)
{
  if (m_Name.isEmpty())
    ATHROW_EX(this, AException::InvalidObject, "AXmlElement does not have a name");

  LIST_AString xparts;
  xpath.split(xparts, '/');
  if (!xparts.size())
    ATHROW(this, AException::InvalidParameter);

  ARope rope;

  AXmlElement *p = NULL;
  if (insert)
    p = _getAndInsert(xparts, this);
  else
    p = _createAndAppend(xparts, this);

  object.emit(*p);
  return *p;
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const size_t value, bool insert)
{
  return addElement(xpath, AString::fromSize_t(value), AXmlData::None, insert);
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const double value, bool insert)
{
  return addElement(xpath, AString::fromDouble(value), AXmlData::None, insert);
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const u8 value, bool insert)
{
  return addElement(xpath, AString::fromU8(value), AXmlData::None, insert);
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const u4 value, bool insert)
{
  return addElement(xpath, AString::fromU4(value), AXmlData::None, insert);
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const char value, bool insert)
{
  return addElement(xpath, AString(value), AXmlData::None, insert);
}

AXmlElement& AXmlElement::addElement(const AString& xpath, const bool value, bool insert)
{
  return addElement(xpath, value ? AString::sstr_True : AString::sstr_False, AXmlData::None, insert);
}

AXmlElement& AXmlElement::addElement(
  const AString& xpath, 
  const char * value, 
  u4 len,                      // = AConstant::npos
  AXmlData::Encoding encoding, // = AXmlData::None
  bool insert                  // = true
)
{
  return addElement(xpath, AString(value, len), encoding, insert);
}

AXmlElement& AXmlElement::addAttribute(const AString& name, const AString& value)
{
  m_Attributes.insert(name, value);

  return *this;
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

AXmlElement& AXmlElement::addAttributes(const AAttributes& attrs)
{
  m_Attributes.append(attrs);

  return *this;
}

AXmlElement& AXmlElement::addData(const AEmittable& data, AXmlData::Encoding encoding)
{
  return addContent(new AXmlData(data, encoding));
}

AXmlElement& AXmlElement::addContent(AXmlNode *pnode)
{
  AASSERT(this, pnode);
  pnode->setParent(this);
  m_Content.push_back(pnode);

  return *this;
}

AXmlElement& AXmlElement::addContent(const AXmlEmittable& data)
{
  data.emit(*this);  
  return *this;
}

AXmlElement& AXmlElement::addComment(const AString& comment)
{
  AXmlInstruction *p = new AXmlInstruction(AXmlInstruction::COMMENT);
  p->useData().set(comment);
  addContent(p);

  return *this;
}

void AXmlElement::emit(AOutputBuffer& target, int indent) const
{
  if (!m_Name.isEmpty())
  {
    //a_Element has a name
    if (indent >= 0)
    {
      if (indent)
        target.append(AString::sstr_CRLF);                                    //a_End of line
      for (int i=0; i<indent; ++i) target.append(AString::sstr_TwoSpaces);    //a_Indentation
    }
    target.append(AXmlDocument::sstr_Start);
    target.append(m_Name);

    //a_Display attributes of this element
    if (m_Attributes.size() > 0)
    {
      target.append(AString::sstr_Space);
      m_Attributes.emit(target);
    }

    NodeContainer::const_iterator cit = m_Content.begin();
    int iSubElementCount = 0;
    if (cit != m_Content.end())
    {
      target.append(AXmlDocument::sstr_End);
      while (cit != m_Content.end())
      {
        if (typeid(*(*cit)) == typeid(AXmlData))
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
        target.append(AString::sstr_CRLF);                                      //a_End of line
        _indent(target, indent);
      }
      target.append(AXmlDocument::sstr_StartEnd);
      target.append(m_Name);
      target.append(AXmlDocument::sstr_End);
    }
    else
    {
      target.append(AXmlDocument::sstr_EndSingular);
    }
  }
  else
  {
    //a_Element has no name, just iterate the children if any
    NodeContainer::const_iterator cit = m_Content.begin();
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

void AXmlElement::emit(AOutputBuffer& target) const
{
  emit(target,-1);
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
  file.readUntilOneOf(m_Name, AXmlDocument::sstr_EndOrWhitespace, false);
  file.skipOver();
  
  //a_Find /> or >
  char c = ' ';
  file.readUntilOneOf(str, AXmlDocument::sstr_End);
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
    if (AConstant::npos == file.readUntilOneOf(str, AXmlDocument::sstr_Start))
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
        file.readUntilOneOf(str, AXmlDocument::sstr_EndOrWhitespace);
        file.skipOver(AXmlDocument::sstr_EndOrWhitespace);                //a_Skip over whitespace and >
        if (str != m_Name)
          ATHROW_EX(this, AException::InvalidData, AString("Close tag </")+str+"> does not match opened tag <"+m_Name+">");
        
        boolEndTagFound = true;
      }
      break;

      //a_Handle embedded CDATA, comments and such
      case '!':
      {
        file.putBack('<');
        str.clear();
        file.readUntil(str, AXmlDocument::sstr_End, true, false);
        if (0 == str.findNoCase(AXmlData::sstr_StartCDATA))
        {
          //a_CDATA found
          AString data;
          str.peek(data, AXmlData::sstr_StartCDATA.getSize());
          data.rremove(AXmlData::sstr_EndCDATA.getSize());
          addData(data, AXmlData::CDataDirect);
        }
        else if (0 == str.findNoCase(AXmlDocument::sstr_StartComment))
        {
          //a_Process a comment
          if (str.getSize() - 3 != str.findNoCase(AXmlDocument::sstr_EndComment))
          {
            //a_Try reading more in case > was contained in the comment
            file.readUntil(str, AXmlDocument::sstr_EndComment, true, false);
          }

          if (str.getSize() - 3 != str.findNoCase(AXmlDocument::sstr_EndComment))
            ATHROW_EX(this, AException::InvalidData, AString("Comment does not terminate with --> in ") +str);

          AAutoPtr<AXmlInstruction> p(new AXmlInstruction(AXmlInstruction::COMMENT));
          str.remove(4);
          str.rremove(3);
          p->parse(str);
          addContent(p.get());
          p.setOwnership(false);
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
        AAutoPtr<AXmlElement> psubElement(new AXmlElement());
        psubElement->fromAFile(file);
        psubElement.setOwnership(false);
        addContent(psubElement.get());
      }
      break;
    }
  }
}

void AXmlElement::toAFile(AFile& file) const
{
  ARope rope;
  emit(rope);
  rope.toAFile(file);
}

bool AXmlElement::hasElements() const
{
  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    //a_Stop when first instance is encountered
    if (NULL != dynamic_cast<const AXmlElement *>(*cit))
      return true;

    ++cit;
  }
  return false;
}

void AXmlElement::find(const AString& path, AXmlNode::NodeContainer& result)
{
  AString strAttribute;
  AString strPath(path);
  if ('/' == strPath.at(0))
    strPath.remove();
  
  size_t pos = path.find('@');
  if (AConstant::npos != pos)
  {
    //a_Extract attribute
    strPath.get(strAttribute, pos);
    strPath.setSize(strPath.getSize() - 1);  //a_Remove the trailing '@'
  }

  //a_Remove the leading name (which should be this element's name)
  LIST_AString listPath;
  strPath.split(listPath, '/');
  if (listPath.front().compare(m_Name))
  {
    //a_First token MUST be the name of this element
    ATHROW_EX(this, AException::ProgrammingError, AString("PATH not found: ")+listPath.front()+" does not match "+m_Name);
  }
  else if (listPath.size() == 1)
  {
    //a_This is where we stop
    result.push_back(this);
    return;
  }

  listPath.pop_front();
  strPath.removeUntil('/');

  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    AXmlElement *p = dynamic_cast<AXmlElement *>(*cit);
    if (p && !p->getName().compare(listPath.front()))
    {
      p->find(strPath, result);
    }
    ++cit;
  }
}

void AXmlElement::find(const AString& path, AXmlNode::ConstNodeContainer& result) const
{
  AString strAttribute;
  AString strPath(path);
  if ('/' == strPath.at(0))
    strPath.remove();
  
  size_t pos = path.find('@');
  if (AConstant::npos != pos)
  {
    //a_Extract attribute
    strPath.get(strAttribute, pos);
    strPath.setSize(strPath.getSize() - 1);  //a_Remove the trailing '@'
  }

  //a_Remove the leading name (which should be this element's name)
  LIST_AString listPath;
  strPath.split(listPath, '/');
  if (listPath.front().compare(m_Name))
  {
    //a_First token MUST be the name of this element
    ATHROW_EX(this, AException::ProgrammingError, ARope("Path must start with the name of the current root element: /")+m_Name);
  }
  else if (listPath.size() == 1)
  {
    //a_This is where we stop
    result.push_back(this);
    return;
  }

  listPath.pop_front();
  strPath.removeUntil('/');

  NodeContainer::const_iterator cit = m_Content.begin();
  while (cit != m_Content.end())
  {
    AXmlElement *p = dynamic_cast<AXmlElement *>(*cit);
    if (p && !p->getName().compare(listPath.front()))
    {
      p->find(strPath, result);
    }
    ++cit;
  }
}

