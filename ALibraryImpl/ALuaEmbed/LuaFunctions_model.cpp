#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AXmlDocument.hpp"
#include "ATemplate.hpp"

/*!
Uses the model to emit content for a path

thisfunction("<path to the AXmlElement to emit>")

@namespace model
@param Path to emit
@return Content at the given path
*/
static int alibrary_Objects_Model_emitContentFromPath(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  lua_pop(L,1);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AString str;
  AXmlElement::CONST_CONTAINER nodes;
  size_t ret = pLuaEmbed->useModel().useRoot().find(xmlpath, nodes);
  for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
  {
    (*cit)->emitContent(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    str.clear();
  }
  return (int)ret;
}

/*!
Emits the entire model as XML

thisfunction([indent = -1])

@namespace model
@param (Optional) if 0 or greater, will indent the output.  Default -1 will inline it.
@return XML string of default AXmlDocument model
*/
static int alibrary_Objects_Model_emitXml(lua_State *L)
{
  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
    lua_pop(L,1);
  }

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AString str;
  pLuaEmbed->useModel().useRoot().emit(str, indent);

  lua_pushlstring(L, str.c_str(), str.getSize());
  return 1;
}

/*!
Emits the entire model as JSON

thisfunction([indent = -1])

@namespace model
@param (Optional) if 0 or greater, will indent the output.  Default -1 will inline it.
@return JSON string of default AXmlDocument model
*/
static int alibrary_Objects_Model_emitJson(lua_State *L)
{
  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
    lua_pop(L,1);
  }

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AString str;
  pLuaEmbed->useModel().useRoot().emitJson(str, indent);

  lua_pushlstring(L, str.c_str(), str.getSize());
  return 1;
}

/*!
Adds a value for an element at the given path, will creates element

thisfunction("element path", "value")

@param Element path
@param Value to set
@return nil
*/
static int alibrary_Objects_Model_addElementText(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  s = luaL_checklstring(L, 2, &len);
  const AString& value = AString::wrap(s, len);
  lua_pop(L,2);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  pLuaEmbed->useModel().useRoot().addElement(xmlpath).addData(value);
  return 0;
}

/*!
Inserts a value for an element at the given path, if exists it will replace otherwise create

thisfunction("element path", "value")

@param Element path
@param Value to set
@return nil
*/
static int alibrary_Objects_Model_insertElementText(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  s = luaL_checklstring(L, 2, &len);
  const AString& value = AString::wrap(s, len);
  lua_pop(L,2);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  pLuaEmbed->useModel().useRoot().addElement(xmlpath, true).addData(value);
  return 0;
}

static const luaL_Reg model_funcs[] = {
  {"emitXml", alibrary_Objects_Model_emitXml},
  {"emitJson", alibrary_Objects_Model_emitJson},
  {"emitContentFromPath", alibrary_Objects_Model_emitContentFromPath},
  {"addElementText", alibrary_Objects_Model_addElementText},
  {"insertElementText", alibrary_Objects_Model_insertElementText},
  {NULL, NULL}
};

LUALIB_API int luaopen_model(lua_State *L)
{
  luaL_register(L, "model", model_funcs);
  return 1;
}
