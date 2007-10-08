#include "pchABase.hpp"
#include "ATemplate.hpp"
#include "ATemplateNode_Code.hpp"
#include "ATemplateNode_Text.hpp"
#include "templateAutoPtr.hpp"
#include "AFile.hpp"

const AString ATemplate::OBJECTNAME_MODEL("_AXmlDocument_Model_");     // AXmlDocument that acts as a model for template lookup

const AString ATemplate::TAG_START("#[",2);
const AString ATemplate::BLOCK_START("]!{",3);
const AString ATemplate::BLOCK_END("}![",3);
const AString ATemplate::TAG_END("]#",2);

#ifdef __DEBUG_DUMP__
void ATemplate::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplate @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Nodes={  ";
  ADebugDumpable::indent(os, indent+2) << "size=" << m_Nodes.size() << std::endl;
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m_Creators={  ";
  ADebugDumpable::indent(os, indent+2) << "size=" << m_Creators.size() << std::endl;
  MAP_CREATORS::const_iterator cit2 = m_Creators.begin();
  while (cit2 != m_Creators.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*cit2).first << std::endl;
    ++cit2;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Creators={";
  m_Objects.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplate::ATemplate(AXmlDocument* pModel, AOutputBuffer* pOutput)
{
  AASSERT_EX(this, pModel, ASWNL("Must provide AXmlDocument type as a model used for template expansion"));
  m_Objects.insert(ATemplate::OBJECTNAME_MODEL, pModel);
  
  if (pOutput)
    mp_Output.reset(pOutput, false);   //a_Do not own the passed output
  else
    mp_Output.reset(new ARope());      //a_Own the created one
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

void ATemplate::registerCreator(const AString& tagName, ATemplateNode::CreatorMethodPtr ptr)
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
  AAutoPtr<ATemplateNode_Text> pText(new ATemplateNode_Text(*this));
  size_t ret = AConstant::npos;
  while(AConstant::npos != (ret = aFile.readUntil(pText->useText(), ATemplate::TAG_START)))
  {
    if (AConstant::npos == (ret = aFile.readUntil(tagName, ATemplate::BLOCK_START)))
    {
      pText->useText().append(ATemplate::TAG_START);
      break;  //a_No more
    }

    //a_Check if this tag is registered
    MAP_CREATORS::iterator it = m_Creators.find(tagName);
    if (m_Creators.end() == it)
    {
      //a_No such tag, add to the current text node and keep going
      pText->useText().append(ATemplate::TAG_START);
      pText->useText().append(tagName);
      pText->useText().append(ATemplate::BLOCK_START);
    }
    else
    {
      //a_Add text first
      m_Nodes.push_back(pText.get());
      pText.setOwnership(false);

      //a_Create new active text node
      pText.reset(new ATemplateNode_Text(*this));

      //a_Create the tag specific node and parse it
      ATemplateNode::CreatorMethodPtr methodptr = (*it).second;
      ATemplateNode *pNode = methodptr(*this, aFile);
      
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

void ATemplate::process()
{
  //a_If this assert failed, theis object was not constructed correctly
  if (!m_Objects.getAsPtr<AXmlDocument>(ATemplate::OBJECTNAME_MODEL))
    ATHROW_EX(this, AException::InvalidObject, ASWNL("Must have AXmlDocument type to be used as a model"));
  
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->process();
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

ABasePtrHolder& ATemplate::useObjects()
{
  return m_Objects;
}

AOutputBuffer& ATemplate::useOutput()
{
  return *mp_Output.get();
}

AXmlDocument& ATemplate::useModel()
{
  AXmlDocument *pDoc = m_Objects.getAsPtr<AXmlDocument>(ATemplate::OBJECTNAME_MODEL);
  if (!pDoc)
    ATHROW_EX(this, AException::InvalidObject, ASWNL("Must have AXmlDocument type to be used as a model"));
  else
    return *pDoc;
}
