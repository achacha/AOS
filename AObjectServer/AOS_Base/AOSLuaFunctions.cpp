#include "pchAOS_Base.hpp"

#include "apiALuaEmbed.hpp"

extern "C"
{
  #include "lauxlib.h"
  #include "lstate.h"
}

#include "AXmlNode.hpp"
#include "AXmlDocument.hpp"
#include "ATemplate.hpp"
#include "AOSContext.hpp"
#include "ALuaEmbed.hpp"

/*!
Insert text at element path, if it doesn't exist, it is created
aos.insertText("<xmlpath>", "<text>")

e.g.
aos.addText("/foo/bar", "something");

results:
<foo>
  <bar>something</bar>
</foo>
*/
static int aos_addText(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaEmbed->useObjects().useAsPtr<AOSContext>("/context");
  AASSERT(NULL, pContext);

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  lua_pop(L,1);

  //a_param 1: text
  s = luaL_checklstring(L, 1, &len);
  const AString& text = AString::wrap(s, len);
  lua_pop(L,1);

  pContext->useOutputRootXmlElement().addElement(xmlpath, text);

  return 0;
}

static const luaL_Reg aos_funcs[] = {
  {"addText", aos_addText},
  {NULL, NULL}
};

extern "C" int luaopen_aos(lua_State *L)
{
  luaL_register(L, "aos", aos_funcs);
  return 1;
}
