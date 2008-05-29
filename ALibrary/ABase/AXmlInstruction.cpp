/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AXmlInstruction.hpp"
#include "AException.hpp"

void AXmlInstruction::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AXmlInstruction @ " << std::hex << this << std::dec << ") {" << std::endl;
  AXmlElement::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "  m_Data=" << m_Data << std::endl;
  AString str;
  m_Attributes.emit(str);
  ADebugDumpable::indent(os, indent) << "  m_Attributes=" << str << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AXmlInstruction::AXmlInstruction(AXmlInstruction::TYPE type, AXmlElement *pParent /* = NULL */) :
  AXmlElement(pParent)
{
  setType(type);
}

AXmlInstruction::AXmlInstruction(const AString& name, AXmlElement *pParent /* = NULL */) :
  AXmlElement(pParent)
{
  if (name.equals("?xml", 4))
    setType(AXmlInstruction::XML_HEADER);
  else if (name.equals("!--", 3))
    setType(AXmlInstruction::COMMENT);
  else if (name.equals("?xml-stylesheet", 15))
    setType(AXmlInstruction::XML_STYLESHEET);
  else if (name.equals("!DOCTYPE", 8))
    setType(AXmlInstruction::DOCTYPE);
  else if (name.equals("!ELEMENT", 8))
    setType(AXmlInstruction::DTD_ELEMENT);
  else
    ATHROW_EX(this, AException::InvalidParameter, AString("Unknown instruction name: ")+name);
}

AXmlInstruction::AXmlInstruction(
  AXmlInstruction::TYPE type, 
  const AXmlAttributes& attrs, 
  const AString& data,      // = AConstant::ASTRING_EMPTY
  AXmlElement *pParent         // = NULL
) :
  AXmlElement(attrs, pParent),
  m_Data(data)
{
  setType(type);
}

AXmlInstruction::AXmlInstruction(const AXmlInstruction& that)
{
  m_Name.assign(that.m_Name);
  m_Attributes.append(that.m_Attributes);
  m_Data.assign(that.m_Data);
  setType(that.m_Type);
}

void AXmlInstruction::setType(AXmlInstruction::TYPE type)
{
  switch(type)
  {
    case XML_HEADER:
      m_Name.assign("?xml", 4);
      break;

    case XML_STYLESHEET:
      m_Name.assign("?xml-stylesheet", 15);
      break;

    case DOCTYPE:
      m_Name.assign("!DOCTYPE", 8);
      break;

    case DTD_ELEMENT:
      m_Name.assign("!ELEMENT", 8);
      break;

    case COMMENT:
      m_Name.assign("!--", 3);
      break;

    default:
      ATHROW(this, AException::InvalidParameter);
  }
  m_Type = type;
}

const AString& AXmlInstruction::getTypeTerminator()
{
  static const AString S_DEFAULT(">", 1);
  static const AString S_XML("?>", 2);
  static const AString S_COMMENT("-->", 3);
  switch(m_Type)
  {
    case XML_HEADER:
      return S_XML;

    case XML_STYLESHEET:
      return S_XML;

    case DOCTYPE:
      return S_DEFAULT;

    case DTD_ELEMENT:
      return S_DEFAULT;

    case COMMENT:
      return S_COMMENT;

    default:
      ATHROW(this, AException::InvalidParameter);
  }
}

AXmlInstruction::TYPE AXmlInstruction::getType() const
{
  return m_Type;
}

void AXmlInstruction::clear()
{
  AXmlElement::clear();
  m_Data.clear();
}

void AXmlInstruction::parse(const AString& contents)
{
  switch (m_Name.at(0))
  {
    case '?':
      m_Attributes.parse(contents);
      break;

    case '!':
      m_Data.assign(contents);
      break;
  }
}

void AXmlInstruction::emitJson(
  AOutputBuffer& target, 
  int // indent = -1
) const
{
  AASSERT(this, !m_Name.isEmpty());
  switch (m_Name.at(0))
  {
    case '!':
      //a_Emit a comment with // ... <CRLF>
      if (COMMENT == m_Type)
      {
        target.append("//", 2);
        target.append(m_Data);
        target.append(AConstant::ASTRING_CRLF);
      }
    break;
  }
}

void AXmlInstruction::emit(AOutputBuffer& target) const
{
  emit(target, -1);
}

void AXmlInstruction::emit(AOutputBuffer& target, int) const
{
  //a_No indent on instructions
  target.append(AXmlElement::sstr_Start);
  
  AASSERT(this, !m_Name.isEmpty());
  target.append(m_Name);
  
  if (m_Attributes.size() > 0)
  {
    target.append(' ');
    m_Attributes.emit(target);
    if (!m_Data.isEmpty())
      target.append(' ');
  }
  if (!m_Data.isEmpty())
  {
    target.append(m_Data);
  }

  switch (m_Name.at(0))
  {
    case '?':
      target.append(AXmlElement::sstr_EndInstruction);
      break;

    case '!':
      if (!m_Name.compare("!--", 3))
      {
        target.append(AXmlElement::sstr_EndComment);
      }
      break;

    default:
      target.append(AXmlElement::sstr_End);
  }
}

bool AXmlInstruction::isElement() const
{
  return false;
}

bool AXmlInstruction::isData() const
{
  return false;
}

bool AXmlInstruction::isInstruction() const
{
  return true;
}
