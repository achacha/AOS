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
    //a_Tag name exists
    target.append(ATemplate::TAG_START);
    target.append(tag);
    target.append(ATemplate::BLOCK_START);

    target.append(m_BlockData);

    target.append(ATemplate::BLOCK_END);
    target.append(tag);
    target.append(ATemplate::TAG_END);
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
    //a_Tag name exists
    aFile.write(ATemplate::TAG_START);
    aFile.write(tag);
    aFile.write(ATemplate::BLOCK_START);

    aFile.write(m_BlockData);

    aFile.write(ATemplate::BLOCK_END);
    aFile.write(tag);
    aFile.write(ATemplate::TAG_END);
  }
}

void ATemplateNode::fromAFile(AFile& aFile)
{
  const AString& tag = getTagName();

  m_BlockData.clear();

  AString endToken;
  if (tag.isEmpty())
  {
    //a_No tag name, read until next tag starts
    endToken.assign(ATemplate::BLOCK_START);

    //a_Read until TAG_START
    if (AConstant::npos == aFile.readUntil(m_BlockData, ATemplate::TAG_START, false, false))
      aFile.readUntilEOF(m_BlockData);  //a_No more delimeters, read to EOF
  }
  else
  {
    //a_End tag delimiter
    endToken.assign(ATemplate::BLOCK_END);
    endToken.append(getTagName());
    endToken.append(ATemplate::TAG_END);

    //a_Read until end token into a string file which is parsed on per-line basis
    //a_If end token not found, read everything to EOF
    if (AConstant::npos == aFile.readUntil(m_BlockData, endToken, true, true))
      aFile.readUntilEOF(m_BlockData);

    _handleDataAfterRead();
  }
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
