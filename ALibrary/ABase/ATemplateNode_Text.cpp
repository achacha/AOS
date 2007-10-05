#include "pchABase.hpp"
#include "ATemplateNode_Text.hpp"
#include "AFile.hpp"
#include "ATemplate.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
void ATemplateNode_Text::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplateNode_Text @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Text='" << m_Text << "'" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNode_Text::ATemplateNode_Text(const AString& text /* = AConstant::ASTRING_EMPTY */) :
  m_Text(text)
{
}

ATemplateNode_Text::ATemplateNode_Text(const ATemplateNode_Text& that) :
  m_Text(that.m_Text)
{
}

void ATemplateNode_Text::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("ATemplateNode_Text",18));

  target.addElement(ASW("text",4), m_Text, AXmlData::CDataSafe);
}

void ATemplateNode_Text::emit(AOutputBuffer& target) const
{
  target.append(m_Text);
}

void ATemplateNode_Text::process(AOutputBuffer& output, const AXmlElement&)
{
  output.append(m_Text);
}

void ATemplateNode_Text::toAFile(AFile& aFile) const
{
  aFile.write(m_Text);
}

void ATemplateNode_Text::fromAFile(AFile& aFile)
{
  //a_Read until TAG_WRAPPER
  if (AConstant::npos == aFile.readUntil(m_Text, ATemplate::TAG_WRAPPER, false, false))
  {
    //a_No more delimeters
    aFile.readUntilEOF(m_Text);
  }
}

AString& ATemplateNode_Text::useText()
{
  return m_Text;
}

