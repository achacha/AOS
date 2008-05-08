
#include "pchABase.hpp"

#include "AXmlData.hpp"
#include "ATextConverter.hpp"
#include "AException.hpp"
#include "AXmlElement.hpp"

const AString AXmlData::sstr_StartCDATA("<![CDATA[");
const AString AXmlData::sstr_EndCDATA("]]>");

void AXmlData::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AXmlData @ " << std::hex << this << std::dec << ") {" << std::endl;
  AXmlElement::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "  m_Encoding=" << m_Encoding << "  m_Data={" << m_Data << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AXmlData::AXmlData()
{
}

AXmlData::AXmlData(const AEmittable& data, AXmlElement::Encoding encoding /*  = AXmlElement::ENC_NONE */, AXmlElement *pParent /* = NULL */) :
  AXmlElement(pParent),
  m_Encoding(encoding)
{
  data.emit(m_Data);
}

AXmlData::AXmlData(const AXmlData& that) :
  AXmlElement(that),
  m_Data(that.m_Data),
  m_Encoding(that.m_Encoding)
{
  //a_Parent not copied
}

AXmlData::~AXmlData()
{
}

AString& AXmlData::useData()
{
  return m_Data;
}

const AString& AXmlData::getData() const
{
  return m_Data;
}

void AXmlData::clear()
{
  AXmlElement::clear();
  m_Data.clear();
  m_Encoding = AXmlElement::ENC_NONE;
}

void AXmlData::setEncoding(AXmlElement::Encoding encoding)
{
  m_Encoding = encoding;
}

void AXmlData::emitJson(AOutputBuffer& target, int indent) const
{
  if (indent >=0) _indent(target, indent);
  target.append("data_:\'",7);
  target.append(m_Data);
  target.append('\'');
}

void AXmlData::emitContent(AOutputBuffer& target) const
{
  if (!m_Data.isEmpty())
    target.append(m_Data);  //a_Direct output
}

void AXmlData::emit(AOutputBuffer& target) const
{
  emit(target, -1);
}

void AXmlData::emit(AOutputBuffer& target, int) const
{
  switch(m_Encoding)
  {
    case AXmlElement::ENC_NONE:
      target.append(m_Data);
    break;

    case AXmlElement::ENC_URL:
      ATextConverter::encodeURL(m_Data, target);
    break;

    case AXmlElement::ENC_CDATADIRECT:
      target.append(sstr_StartCDATA);
      target.append(m_Data);
      target.append(sstr_EndCDATA);
    break;

    case AXmlElement::ENC_CDATASAFE:
      target.append(sstr_StartCDATA);
      ATextConverter::makeCDataSafe(m_Data, target);
      target.append(sstr_EndCDATA);
    break;

    case AXmlElement::ENC_XMLSAFE:
      ATextConverter::makeHtmlSafe(m_Data, target);
    break;

    case AXmlElement::ENC_BASE64:
      ATextConverter::encodeBase64(m_Data, target);
    break;
    
    case AXmlElement::ENC_CDATAHEXDUMP:
      target.append(sstr_StartCDATA);
      ATextConverter::convertStringToHexDump(m_Data, target);
      target.append(sstr_EndCDATA);
    break;

    default:
      ATHROW(this, AException::ProgrammingError);
  }
}

bool AXmlData::isElement() const
{
  return false;
}

bool AXmlData::isData() const
{
  return true;
}

bool AXmlData::isInstruction() const
{
  return false;
}
