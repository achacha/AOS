#include "pchALuaEmbed.hpp"
#include "ATemplateNodeHandler_LUA.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "AFile_AString.hpp"
#include "ATemplate.hpp"

const AString ATemplateNodeHandler_LUA::TAGNAME("LUA",3);

#ifdef __DEBUG_DUMP__
void ATemplateNodeHandler_LUA::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Lua={" << std::endl;
  m_Lua.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

#ifdef __DEBUG_DUMP__
void ATemplateNodeHandler_LUA::Node::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNodeHandler_LUA::ATemplateNodeHandler_LUA(u4 maskLibrariesToLoad) :
  m_Lua(maskLibrariesToLoad)
{
}

ATemplateNodeHandler_LUA::~ATemplateNodeHandler_LUA()
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

ALuaEmbed& ATemplateNodeHandler_LUA::useLua()
{
  return m_Lua;
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

void ATemplateNodeHandler_LUA::Node::process(ABasePtrHolder& objects, AOutputBuffer& output)
{
  ATemplateNodeHandler_LUA *pHandler = dynamic_cast<ATemplateNodeHandler_LUA *>(mp_Handler);
  AASSERT(this, pHandler);

  pHandler->useLua().execute(m_BlockData, objects, output);
}
