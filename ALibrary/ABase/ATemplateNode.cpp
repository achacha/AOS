#include "pchABase.hpp"
#include "ATemplateNode.hpp"
#include "ATemplate.hpp"
#include "AXmlElement.hpp"
#include "AFile.hpp"

#ifdef __DEBUG_DUMP__
void ATemplateNode::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_BlockData={" << std::endl;
  m_BlockData.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
  
}
#endif

ATemplateNode::ATemplateNode(ATemplate& t) :
  m_ParentTemplate(t)
{
}

ATemplateNode::ATemplateNode(const ATemplateNode& that) :
  m_ParentTemplate(that.m_ParentTemplate),
  m_BlockData(that.m_BlockData)
{
}

ATemplateNode::~ATemplateNode()
{
}

void ATemplateNode::emitXml(AXmlElement& target) const
{
  AASSERT(this, !target.useName().isEmpty());
  target.addElement(ASW("script",6)).addData(m_BlockData, AXmlData::CDataDirect);
}

void ATemplateNode::emit(AOutputBuffer& target) const
{
  target.append(ATemplate::TAG_START);
  target.append(getTagName());
  target.append(ATemplate::BLOCK_START);
  target.append(AConstant::ASTRING_CRLF);

  target.append(m_BlockData);

  target.append(ATemplate::BLOCK_END);
  target.append(getTagName());
  target.append(ATemplate::TAG_END);
}

void ATemplateNode::toAFile(AFile& aFile) const
{
  aFile.write(ATemplate::TAG_START);
  aFile.write(getTagName());
  aFile.writeLine(ATemplate::BLOCK_START);

  aFile.write(m_BlockData);

  aFile.write(ATemplate::BLOCK_END);
  aFile.write(getTagName());
  aFile.write(ATemplate::TAG_END);
}

void ATemplateNode::fromAFile(AFile& aFile)
{
  //a_End tag delimiter
  AString endToken(ATemplate::BLOCK_END);
  endToken.append(getTagName());
  endToken.append(ATemplate::TAG_END);
  
  //a_Read until end token into a string file which is parsed on per-line basis
  m_BlockData.clear();
  if (AConstant::npos == aFile.readUntil(m_BlockData, endToken, true, true))
    ATHROW(this, AException::EndOfBuffer);
}

const AString& ATemplateNode::getBlockData() const
{
  return m_BlockData;
}
