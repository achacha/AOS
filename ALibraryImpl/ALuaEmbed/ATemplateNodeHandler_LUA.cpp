#include "pchALuaEmbed.hpp"
#include "ATemplateNodeHandler_LUA.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "AFile_AString.hpp"
#include "ATemplate.hpp"

const AString ATemplateNodeHandler_LUA::TAGNAME("LUA",3);

void ATemplateNodeHandler_LUA::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void ATemplateNodeHandler_LUA::Node::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplateNodeHandler_LUA::ATemplateNodeHandler_LUA(u4 maskLibrariesToLoad) :
  m_LibrariesToLoad(maskLibrariesToLoad)
{
}

ATemplateNodeHandler_LUA::~ATemplateNodeHandler_LUA()
{
}

void ATemplateNodeHandler_LUA::addUserDefinedLibrary(ALuaEmbed::LUA_OPENLIBRARY_FPTR fptr)
{
  m_UserDefinedLibFunctions.push_back(fptr);
}

void ATemplateNodeHandler_LUA::init()
{
}

void ATemplateNodeHandler_LUA::deinit()
{
}

const AString& ATemplateNodeHandler_LUA::getTagName() const
{
  return ATemplateNodeHandler_LUA::TAGNAME;
}

ATemplateNode *ATemplateNodeHandler_LUA::create(AFile& file)
{
  ATemplateNodeHandler_LUA::Node *pNode = new ATemplateNodeHandler_LUA::Node(this);
  pNode->fromAFile(file);
  return pNode;
}

///////////////////////////////////////// ATemplateNodeHandler_LUA::Node ///////////////////////////////////////////////////////////////////////////

ATemplateNodeHandler_LUA::Node::Node(ATemplateNodeHandler *pHandler) :
  ATemplateNode(pHandler)
{
}

ATemplateNodeHandler_LUA::Node::Node(const ATemplateNodeHandler_LUA::Node& that) :
  ATemplateNode(that)
{
}

ATemplateNodeHandler_LUA::Node::~Node()
{
}

void ATemplateNodeHandler_LUA::Node::process(ATemplateContext& context)
{
  ATemplateNodeHandler_LUA *pHandler = dynamic_cast<ATemplateNodeHandler_LUA *>(mp_Handler);
  AASSERT(this, pHandler);

  ALuaEmbed lua(pHandler->m_LibrariesToLoad);

  //a_Load queued up user defined libraries
  for (ATemplateNodeHandler_LUA::USER_DEFINED_FPTRS::iterator it = pHandler->m_UserDefinedLibFunctions.begin(); it != pHandler->m_UserDefinedLibFunctions.end(); ++it)
    lua.loadUserLibrary(*it);

  if (!lua.execute(m_BlockData, context))
  {
    context.useOutput().append(" block={{{\r\n",12);
    context.useOutput().append(m_BlockData);
    context.useOutput().append("}}}\r\n",5);
  }
}
