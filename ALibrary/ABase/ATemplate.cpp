#include "pchABase.hpp"
#include "ATemplate.hpp"
#include "ATemplateNode_Code.hpp"
#include "ATemplateNode_Text.hpp"
#include "templateAutoPtr.hpp"
#include "AFile.hpp"

const AString ATemplate::TAG_WRAPPER("%%",2);
const AString ATemplate::BLOCK_START("%%{",3);
const AString ATemplate::BLOCK_END("}%%",3);

ATemplate::MAP_CREATORS ATemplate::m_Creators;

#ifdef __DEBUG_DUMP__
void ATemplate::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplate @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Nodes={" << std::endl;
  ADebugDumpable::indent(os, indent+2) << "size=" << m_Nodes.size() << std::endl;
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m_Creators={" << std::endl;
  ADebugDumpable::indent(os, indent+2) << "size=" << m_Creators.size() << std::endl;
  MAP_CREATORS::const_iterator cit2 = m_Creators.begin();
  while (cit2 != m_Creators.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*cit2).first << std::endl;
    ++cit2;
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

ATemplate::RegisterWithTemplateParser::RegisterWithTemplateParser(const AString& tagName, ATemplateNode::CreatorMethodPtr pMethod)
{
  ATemplate::_registerCreator(tagName, pMethod);
}

void ATemplate::_registerCreator(const AString& tagName, ATemplateNode::CreatorMethodPtr ptr)
{
  if (m_Creators.find(tagName) == m_Creators.end())
  {
    m_Creators[tagName] = ptr;
  }
  else
    ATHROW_EX(NULL, AException::DataConflict, tagName);
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
  AString tagName;
  AAutoPtr<ATemplateNode_Text> pText(new ATemplateNode_Text());
  size_t ret;
  while(AConstant::npos != (ret = aFile.readUntil(pText->useText(), ATemplate::TAG_WRAPPER)))
  {
    aFile.readUntil(tagName, ATemplate::BLOCK_START);

    //a_Check if this tag is registered
    MAP_CREATORS::iterator it = m_Creators.find(tagName);
    if (m_Creators.end() == it)
    {
      //a_No such tag, add to the current text node and keep going
      pText->useText().append(ATemplate::TAG_WRAPPER);
      pText->useText().append(tagName);
      pText->useText().append(ATemplate::BLOCK_START);
    }
    else
    {
      //a_Add text first
      m_Nodes.push_back(pText.get());
      pText.setOwnership(false);

      //a_Create new active text node
      pText.reset(new ATemplateNode_Text());

      //a_Create the tag specific node and parse it
      ATemplateNode::CreatorMethodPtr methodptr = (*it).second;
      ATemplateNode *pNode = methodptr(aFile);
      
      //a_Add parsed node
      m_Nodes.push_back(pNode);
    }
    tagName.clear();
  }
  if (AConstant::npos == ret)
  {
    //a_Read to EOF
    aFile.readUntilEOF(pText->useText());

    //a_Leftover text added if not empty
    if (!pText->useText().isEmpty())
    {
      m_Nodes.push_back(pText.get());
      pText.setOwnership(false);
    }
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

void ATemplate::emitXml(AXmlElement& target) const
{
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->emitXml(target);
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
