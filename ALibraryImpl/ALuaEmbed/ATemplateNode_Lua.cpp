#include "pchALuaEmbed.hpp"
#include "ATemplateNode_Lua.hpp"
#include "AObjectContainer.hpp"
#include "ATemplate.hpp"
#include "ALuaEmbed.hpp"

const AString ATemplateNode_Lua::TAGNAME("LUA",3);
const AString ATemplateNode_Lua::OBJECTNAME_LUA("_GLOBAL_ALuaEmbed_");

ATemplateNode_Lua::ATemplateNode_Lua(ATemplate& t) :
  ATemplateNode(t)
{
  //a_Make sure Lua interpreter is attached to the ATemplate since it's per instance based
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
  ATemplateNode(that)
{
}

ATemplateNode_Lua::~ATemplateNode_Lua()
{
}

const AString& ATemplateNode_Lua::getTagName() const
{
  return TAGNAME;
}

ATemplateNode* ATemplateNode_Lua::create(ATemplate& t, AFile& file)
{
  ATemplateNode_Lua *p = new ATemplateNode_Lua(t);
  p->fromAFile(file);
  return p;
}

void ATemplateNode_Lua::process()
{
  ALuaEmbed *pLua = m_ParentTemplate.useObjects().getAsPtr<ALuaEmbed>(ATemplateNode_Lua::OBJECTNAME_LUA);
  AASSERT(this, pLua);
  if (pLua)
  {
    pLua->execute(m_BlockData);
  }
}
