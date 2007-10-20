#include "pchAOS_Base.hpp"

#include "apiALuaEmbed.hpp"

extern "C"
{
  #include "lauxlib.h"
  #include "lstate.h"
}

#include "AXmlDocument.hpp"
#include "ATemplate.hpp"
#include "AOSContext.hpp"
#include "ALuaEmbed.hpp"

/*!
Insert text at element path, if it doesn't exist, it is created
aos.addText("<xmlpath>", "<text>")

Call:
aos.addText("/foo/bar", "something");

Results:
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
  AOSContext *pContext = pLuaEmbed->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 2, ASWNL("Not enough actual parameters, expected at least 2"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  //a_param 1: text
  s = luaL_checklstring(L, 2, &len);
  const AString& text = AString::wrap(s, len);
  lua_pop(L,2);

  pContext->useOutputRootXmlElement().addElement(xmlpath, text);

  return 0;
}

/*!
Call AXmlElement::emitContentFromPath on the xmlpath specified
If the path points to more than 1 node/element then a list of values is returned
Emit content call will gather all text blocks from the node and sub-nodes

aos.emitContent("<xmlpath">);

Given:
<root>
  <filename>foo.txt</filename>
  <filename>
    <filepart>bar<filepart>
    <ext>.txt</ext>
  </filename>
  <filename>baz.txt</filename>
</root>


Call:
  print( aos.emitContent("/root/filename") );

Return (3 items):
  foo.txt
  bar.txt
  baz.txt
*/
static int aos_emitContent(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaEmbed->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  lua_pop(L,1);

  AXmlElement::ConstNodeContainer nodes;
  pContext->useOutputRootXmlElement().find(xmlpath, nodes);
  AString str;
  for (AXmlElement::ConstNodeContainer::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    (*it)->emitContent(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    str.clear();
  }
  
  return (int)nodes.size();
}

/*!
Gets HTTP request header parameter

Example:
aos.getRequestHeader("Host");
Returns:
www.someserver.com

Returns:
  item corresponding to request header if found
  nothing if not found
*/
static int aos_getRequestHeader(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaEmbed->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& name = AString::wrap(s, len);
  lua_pop(L,1);

  AString str;
  if (pContext->useRequestHeader().getPairValue(name, str))
  {
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  return 0;
}

/*!
Sets HTTP response header parameter

Example:
aos.setResponseHeader("Set-Cookie", "name=foo; max-ago=1000;");

Returns nothing
*/
static int aos_setResponseHeader(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaEmbed->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& name = AString::wrap(s, len);

  //a_param 1: text
  s = luaL_checklstring(L, 2, &len);
  const AString& value = AString::wrap(s, len);
  lua_pop(L,2);

  pContext->useResponseHeader().setPair(name, value);
  return 0;
}

static const luaL_Reg aos_funcs[] = {
  {"addText", aos_addText},
  {"emitContent", aos_emitContent},
  {"getRequestHeader", aos_getRequestHeader},
  {"setResponseHeader", aos_setResponseHeader},
  {NULL, NULL}
};

extern "C" int luaopen_aos(lua_State *L)
{
  luaL_register(L, "aos", aos_funcs);
  return 1;
}