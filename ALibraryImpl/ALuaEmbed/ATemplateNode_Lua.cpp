#include "pchALuaEmbed.hpp"
#include "ATemplateNode_Lua.hpp"
#include "AObjectContainer.hpp"
#include "ATemplate.hpp"

#define TAGNAME ASW("LUA",3)

//a_Register with ATemplate
ATEMPLATE_REGISTER_NODE(TAGNAME, ATemplateNode_Lua);

#ifdef __DEBUG_DUMP__
void ATemplateNode_Lua::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplateNode_Lua @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << m_Script << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNode_Lua::ATemplateNode_Lua(const ATemplateNode_Lua& that) :
  m_Script(that.m_Script)
{
}

ATemplateNode* ATemplateNode_Lua::create(AFile& file)
{
  ATemplateNode_Lua *p = new ATemplateNode_Lua();
  p->fromAFile(file);
  return p;
}

void ATemplateNode_Lua::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("ATemplateNode_Lua",18));

  target.addElement(ASW("script",6)).addData(m_Script, AXmlData::CDataDirect);
}

void ATemplateNode_Lua::emit(AOutputBuffer& target) const
{
  target.append(ATemplate::TAG_WRAPPER);
  target.append(TAGNAME);
  target.append(ATemplate::BLOCK_START);
  target.append(AConstant::ASTRING_CRLF);

  target.append(m_Script);

  target.append(ATemplate::BLOCK_END);
  target.append(TAGNAME);
  target.append(ATemplate::TAG_WRAPPER);
}

void ATemplateNode_Lua::toAFile(AFile& aFile) const
{
  aFile.write(ATemplate::TAG_WRAPPER);
  aFile.write(TAGNAME);
  aFile.writeLine(ATemplate::BLOCK_START);

  aFile.write(m_Script);

  aFile.write(ATemplate::BLOCK_END);
  aFile.write(TAGNAME);
  aFile.write(ATemplate::TAG_WRAPPER);
}

void ATemplateNode_Lua::fromAFile(AFile& aFile)
{
  //a_End tag delimiter is }%%CODE%%
  AString endToken(ATemplate::BLOCK_END);
  endToken.append(TAGNAME);
  endToken.append(ATemplate::TAG_WRAPPER);
  
  //a_Read until end token into a string file which is parsed on per-line basis
  if (AConstant::npos != aFile.readUntil(m_Script, endToken, true, true))
    ATHROW(this, AException::EndOfBuffer);
}

const AString& ATemplateNode_Lua::getScript() const
{
  return m_Script;
}

void ATemplateNode_Lua::process(AOutputBuffer& output, const AXmlElement& root)
{
  //TODO: Execute Lua script
}
