#include "pchALuaEmbed.hpp"
#include "ATemplateNode_Lua.hpp"
#include "AObjectContainer.hpp"
#include "ATemplate.hpp"
#include "ALuaEmbed.hpp"

const AString ATemplateNode_Lua::TAGNAME("LUA",3);
const AString ATemplateNode_Lua::OBJECTNAME_LUA("_GLOBAL_ALuaEmbed_");

#ifdef __DEBUG_DUMP__
void ATemplateNode_Lua::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplateNode_Lua @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << m_Script << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNode_Lua::ATemplateNode_Lua(ATemplate& t) :
  ATemplateNode(t)
{
  ABase *p = t.useObjects().get(ATemplateNode_Lua::OBJECTNAME_LUA);
  if (!p)
  {
    ALuaEmbed *pLua = new ALuaEmbed(t.useObjects(), t.useOutput(), ALuaEmbed::LUALIB_BASE);
    t.useObjects().insertWithOwnership(ATemplateNode_Lua::OBJECTNAME_LUA, pLua);
  }
  else
  {
    AASSERT(this, NULL != dynamic_cast<ALuaEmbed *>(p));
  }
}

ATemplateNode_Lua::ATemplateNode_Lua(const ATemplateNode_Lua& that) :
  ATemplateNode(that),
  m_Script(that.m_Script)
{
}

ATemplateNode_Lua::~ATemplateNode_Lua()
{
}

ATemplateNode* ATemplateNode_Lua::create(ATemplate& t, AFile& file)
{
  ATemplateNode_Lua *p = new ATemplateNode_Lua(t);
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
  if (AConstant::npos == aFile.readUntil(m_Script, endToken, true, true))
    ATHROW(this, AException::EndOfBuffer);
}

const AString& ATemplateNode_Lua::getScript() const
{
  return m_Script;
}

void ATemplateNode_Lua::process()
{
  ALuaEmbed *pLua = m_ParentTemplate.useObjects().getAsPtr<ALuaEmbed>(ATemplateNode_Lua::OBJECTNAME_LUA);
  AASSERT(this, pLua);
  if (pLua)
  {
    pLua->execute(m_Script);
  }
}
