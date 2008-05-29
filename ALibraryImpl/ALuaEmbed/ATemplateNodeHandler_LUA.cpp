/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchALuaEmbed.hpp"
#include "ATemplateNodeHandler_LUA.hpp"
#include "ALuaTemplateContext.hpp"

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

ATemplateNodeHandler_LUA::ATemplateNodeHandler_LUA()
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

void ATemplateNodeHandler_LUA::init()
{
}

void ATemplateNodeHandler_LUA::deinit()
{
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

void ATemplateNodeHandler_LUA::Node::process(ATemplateContext& context, AOutputBuffer& output)
{
  ALuaTemplateContext *pLuaTemplateContext = dynamic_cast<ALuaTemplateContext *>(&context);
  if (pLuaTemplateContext)
  {
    //a_Have a Lua context with its own interpreter
    if (!pLuaTemplateContext->useLua().execute(m_BlockData, context, output))
    {
      output.append(" block(context)={{{\r\n",21);
      output.append(m_BlockData);
      output.append("}}}\r\n",5);
    }
  }
  else
  {
    //a_Create a temporary interpreter, this is the less efficient way of doing it
    ALuaEmbed lua;
    if (!lua.execute(m_BlockData, context, output))
    {
      output.append(" block(local)={{{\r\n",19);
      output.append(m_BlockData);
      output.append("}}}\r\n",5);
    }
  }
}
