#include "pchALuaEmbed.hpp"
#include "ALibraryFunctions.hpp"
#include "ALuaEmbed.hpp"
#include "AXmlDocument.hpp"

extern "C"
{
#include "lstate.h"
}

static void luaA_stringappender(lua_State *L, const char *s)
{
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  if (pLuaEmbed)
  {
    pLuaEmbed->useOutputBuffer().append(s);
  }
}

/*!
thisfunction("<object path of the AXmlDocument type>", "<path to the AXmlNode to emit>")
returns: content at the node path
*/
static int alibrary_emitXmlDocumentFromPath(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& objpath = AString::wrap(s, len);

  s = luaL_checklstring(L, 2, &len);
  const AString& xmlpath = AString::wrap(s, len);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AXmlDocument *pDoc = pLuaEmbed->useObjects().getAsPtr<AXmlDocument>(objpath);
  AASSERT(NULL, pDoc);
  AString str;

  AXmlNode::ConstNodeContainer nodes;
  size_t ret = pDoc->useRoot().find(xmlpath, nodes);
  for (AXmlNode::ConstNodeContainer::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
  {
    (*cit)->emitContent(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    str.clear();
  }
  return ret;
}

/*!
thisfunction("<object path of AEmittable type>"")
returns: content at the node path
*/
static int alibrary_emit(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& objpath = AString::wrap(s, len);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AObjectBase *pObj = pLuaEmbed->useObjects().getObject(objpath);
  if (pObj)
  {
    AString str;
    pObj->emit(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else
    return 0;
}

/*!
thisfunction("<object path of AXmlElement type>", [indent = -1])
returns: XML string of the object assuming it implements AXmlEmittable interface
*/
static int alibrary_emitXml(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& objpath = AString::wrap(s, len);
  lua_pop(L, 1);

  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
    lua_pop(L,1);
  }

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AXmlElement *p = pLuaEmbed->useObjects().getAsPtr<AXmlElement>(objpath);
  if (p)
  {
    AString str;
    p->emit(str, indent);

    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else
    return 0;
}

/*!
thisfunction("<object path of AXmlElement type>", [indent = -1])
returns: JSON string of the object assuming it implements AXmlEmittable interface
*/
static int alibrary_emitJson(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& objpath = AString::wrap(s, len);
  lua_pop(L, 1);

  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
    lua_pop(L,1);
  }

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AXmlElement *p = pLuaEmbed->useObjects().getAsPtr<AXmlElement>(objpath);
  if (p)
  {
    AString str;
    p->emitJson(str, indent);

    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else
    return 0;
}

/*!
print(...) to outputbuffer attached to ALuaEmbed
Does not append trailing EOL
*/
static int alibrary_print(lua_State *L)
{
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  lua_getglobal(L, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    lua_pushvalue(L, -1);  /* function to be called */
    lua_pushvalue(L, i);   /* value to print */
    lua_call(L, 1, 1);
    s = lua_tostring(L, -1);  /* get result */
    if (s == NULL)
      return luaL_error(L, LUA_QL("tostring") " must return a string to "
      LUA_QL("print"));

    luaA_stringappender(L, s);
    lua_pop(L, 1);
    if (i<n) luaA_stringappender(L, ",");
  }

  return 0;
}

static const luaL_Reg alibrary_funcs[] = {
  {"print", alibrary_print},
  {"emit", alibrary_emit},
  {"emitXml", alibrary_emitXml},
  {"emitJson", alibrary_emitJson},
  {"emitXmlDocumentFromPath", alibrary_emitXmlDocumentFromPath},
  {NULL, NULL}
};

LUALIB_API int luaopen_alibrary(lua_State *L)
{
  luaL_register(L, LUA_ALIBRARYNAME, alibrary_funcs);
  return 1;
}
