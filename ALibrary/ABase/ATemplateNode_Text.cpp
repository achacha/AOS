#include "pchABase.hpp"
#include "ATemplateNode_Text.hpp"
#include "AFile.hpp"
#include "ATemplate.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
void ATemplateNode_Text::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplateNode_Text @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_strText='" << m_strText << "'" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNode_Text::ATemplateNode_Text(const AString& text /* = AString::sstr_Empty */) :
  m_strText(text)
{
}

ATemplateNode_Text::ATemplateNode_Text(const ATemplateNode_Text& that) :
  m_strText(that.m_strText)
{
}

void ATemplateNode_Text::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("ATemplateNode_Text",18));

  target.addElement(ASW("text",4), m_strText, AXmlData::CDataSafe);
}

void ATemplateNode_Text::emit(AOutputBuffer& target) const
{
  target.append(m_strText);
}

void ATemplateNode_Text::process(AOutputBuffer& output, const AXmlElement&)
{
  output.append(m_strText);
}

void ATemplateNode_Text::toAFile(AFile& aFile) const
{
  aFile.write(m_strText);
}

void ATemplateNode_Text::fromAFile(AFile& aFile)
{
  if (AConstant::npos == aFile.readUntil(m_strText, ATemplate::sstr_CodeStart, true, true))
  {
    //a_No more delimeters
    aFile.readUntilEOF(m_strText);
  }
}

AString& ATemplateNode_Text::useText()
{
  return m_strText;
}

