#include "pchABase.hpp"
#include "ATemplate.hpp"
#include "ATemplateNode_Code.hpp"
#include "ATemplateNode_Text.hpp"
#include "templateAutoPtr.hpp"
#include "AFile.hpp"

const AString ATemplate::sstr_CodeStart("<!--[");
const AString ATemplate::sstr_CodeEnd("]-->");

#ifdef __DEBUG_DUMP__
void ATemplate::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplate @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Nodes {" << std::endl;
  ADebugDumpable::indent(os, indent+2) << "size=" << (u4)m_Nodes.size() << std::endl;
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplate::ATemplate()
{
}

ATemplate::~ATemplate()
{
  try
  {
    NODES::iterator it = m_Nodes.begin();
    while (it != m_Nodes.end())
    {
      delete (*it);
      ++it;
    }
  } catch(...) {}
}

void ATemplate::toAFile(AFile& aFile) const
{
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->toAFile(aFile);
    ++cit;
  }
}

void ATemplate::fromAFile(AFile& aFile)
{
  AAutoPtr<ATemplateNode_Text> pText(new ATemplateNode_Text());
  AAutoPtr<ATemplateNode_Code> pCode(new ATemplateNode_Code());
  while (aFile.isNotEof())
  {
    //a_Read text until start is found
    pText->fromAFile(aFile);
    if (!pText->useText().isEmpty())
    {
      m_Nodes.push_back(pText.get());
      pText.setOwnership(false);
      pText.reset(new ATemplateNode_Text());
    }

    if (aFile.isNotEof())
    {
      //a_Code delimeter found
      pCode->fromAFile(aFile);
      if (!pCode->isEmpty())
      {
        m_Nodes.push_back(pCode.get());
        pCode.setOwnership(false);
        pCode.reset(new ATemplateNode_Code());
      }
    }
    else
      break;
  }
}

void ATemplate::process(AOutputBuffer& target, const AXmlElement& root)
{
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->process(target, root);
    ++cit;
  }
}

void ATemplate::emit(AXmlElement& target) const
{
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->emit(target);
    ++cit;
  }
}

void ATemplate::emit(AOutputBuffer& target) const
{
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->emit(target);
    ++cit;
  }
}
