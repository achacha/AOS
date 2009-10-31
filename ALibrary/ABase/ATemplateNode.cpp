/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ATemplateNode.hpp"
#include "ATemplate.hpp"
#include "ATemplateNodeHandler.hpp"
#include "AXmlElement.hpp"
#include "AFile.hpp"

void ATemplateNode::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  if (mp_Handler)
  {
    ADebugDumpable::indent(os, indent+1) << "mp_Handler={"<< std::endl;
    mp_Handler->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_Handler=NULL" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_BlockData={" << std::endl;
  m_BlockData.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
  
}

ATemplateNode::ATemplateNode(ATemplateNodeHandler *pHandler) :
  mp_Handler(pHandler)
{
}

ATemplateNode::ATemplateNode(const ATemplateNode& that) :
  mp_Handler(that.mp_Handler),
  m_BlockData(that.m_BlockData)
{
}

ATemplateNode::~ATemplateNode()
{
}

AXmlElement& ATemplateNode::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  thisRoot.addAttribute(ASW("tag",3), getTagName());
  thisRoot.addElement(ASW("data",4)).addData(m_BlockData, AXmlElement::ENC_CDATADIRECT);

  return thisRoot;
}

void ATemplateNode::emit(AOutputBuffer& target) const
{
  const AString& tag = getTagName();
  if (tag.isEmpty())
  {
    //a_No tag, just output the body
    target.append(m_BlockData);
  }
  else
  {
    if (m_BlockData.isEmpty())
    {
      target.append(AXmlElement::sstr_Start);
      target.append(tag);
      if (m_Attributes.size() > 0)
      {
        target.append(' ');
        m_Attributes.emit(target);
      }
      target.append(AXmlElement::sstr_EndSingular);
    }
    else
    {
      target.append(AXmlElement::sstr_Start);
      target.append(tag);
      if (m_Attributes.size() > 0)
      {
        target.append(' ');
        m_Attributes.emit(target);
      }
      target.append(AXmlElement::sstr_End);

      target.append(m_BlockData);

      target.append(AXmlElement::sstr_StartEnd);
      target.append(tag);
      target.append(AXmlElement::sstr_End);
    }
  }
}

void ATemplateNode::toAFile(AFile& aFile) const
{
  const AString& tag = getTagName();
  if (tag.isEmpty())
  {
    //a_No tag, just output the body
    aFile.write(m_BlockData);
  }
  else
  {
    if (m_BlockData.isEmpty())
    {
      aFile.write(AXmlElement::sstr_Start);
      aFile.write(tag);
      if (m_Attributes.size() > 0)
      {
        aFile.write(' ');
        m_Attributes.toAFile(aFile);
      }
      aFile.write(AXmlElement::sstr_EndSingular);
    }
    else
    {
      aFile.write(AXmlElement::sstr_Start);
      aFile.write(tag);
      if (m_Attributes.size() > 0)
      {
        aFile.write(' ');
        m_Attributes.toAFile(aFile);
      }
      aFile.write(AXmlElement::sstr_End);

      aFile.write(m_BlockData);

      aFile.write(AXmlElement::sstr_StartEnd);
      aFile.write(tag);
      aFile.write(AXmlElement::sstr_End);
    }
  }
}

void ATemplateNode::fromAFile(AFile& aFile)
{
  AString endToken;
  endToken.append('/');
  endToken.append(getTagName());

  m_BlockData.clear();

  //a_Locate end tag in file if not found throw exception, MUST have end
  if (AConstant::npos == aFile.readUntil(m_BlockData, endToken))
    ATHROW_EX(this, AException::EndOfBuffer, endToken);
  
  m_BlockData.rremoveUntilNotOneOf(AXmlElement::sstr_StartOrWhitespace);

  aFile.skipUntilNotOneOf(AXmlElement::sstr_EndSingular);

  //a_Allow derived nodes to process data after read
  _handleDataAfterRead();
}

void ATemplateNode::_handleDataAfterRead()
{
}

AString& ATemplateNode::useBlockData()
{
  return m_BlockData;
}

const AString& ATemplateNode::getTagName() const
{
  return (mp_Handler ? mp_Handler->getTagName() : AConstant::ASTRING_EMPTY);
}

void ATemplateNode::process(ATemplateContext& context, AOutputBuffer& output)
{
  m_BlockData.emit(output);
}

AXmlAttributes& ATemplateNode::useAttributes()
{
  return m_Attributes;
}

const AXmlAttributes& ATemplateNode::getAttributes() const
{
  return m_Attributes;
}
