#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AXmlDocument.hpp"
#include "ATemplate.hpp"

/*!
Uses the model to emit content for a path

thisfunction("<path to the AXmlElement to emit>", separate)

@namespace model
@param Path to emit
@param if non-nil then each element found will be returned by itself
@return Content at the given path or nil if element does not exist
*/
static int alibrary_Objects_Model_emitContentFromPath(lua_State *L)
{
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  int mode = 0;
  if (lua_gettop(L) > 1)
    mode = luaL_checkint(L, 2);

  AXmlElement::CONST_CONTAINER nodes;
  size_t ret = pLuaEmbed->useModel().useRoot().find(xmlpath, nodes);

  if (mode)
  {
    //a_Return each as separate values
    AString str;
    for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      (*cit)->emitContent(str);
      lua_pushlstring(L, str.c_str(), str.getSize());
      str.clear();
    }
    return (int)ret;
  }
  else
  {
    if (ret > 0)
    {
      //a_Return content concatinated
      ARope rope;
      for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
        (*cit)->emitContent(rope);

      const AString& str = rope.toAString();
      lua_pushlstring(L, str.c_str(), str.getSize());
      
      return 1;
    }
    else
      return 0;
  }
}

/*!
Uses the model to determine if element exists

thisfunction("<path to the AXmlElement>")

@namespace model
@param Path to element
@return true if exists, false otherwise
*/
static int alibrary_Objects_Model_existElement(lua_State *L)
{
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  if (pLuaEmbed->useModel().useRoot().exists(xmlpath))
    lua_pushboolean(L, 1);
  else
    lua_pushboolean(L, 0);

  return 1;
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
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
  }

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
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
  }

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
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  AXmlElement &e = pLuaEmbed->useModel().useRoot().addElement(xmlpath);
  if (lua_gettop(L) > 1)
  {
    s = luaL_checklstring(L, 2, &len);
    const AString& value = AString::wrap(s, len);
    e.addData(value);
  }
  return 0;
}

/*!
Sets a value for an element at the given path, if exists it will replace otherwise create

thisfunction("element path", "value")

@param Element path
@param Value to set, if not specified only element is created
@return nil
*/
static int alibrary_Objects_Model_setElementText(lua_State *L)
{
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  
  AXmlElement &e = pLuaEmbed->useModel().useRoot().overwriteElement(xmlpath);
  if (lua_gettop(L) > 1)
  {
    s = luaL_checklstring(L, 2, &len);
    const AString& value = AString::wrap(s, len);

    e.addData(value);
  }

  return 0;
}

static const luaL_Reg model_funcs[] = {
  {"emitXml", alibrary_Objects_Model_emitXml},
  {"emitJson", alibrary_Objects_Model_emitJson},
  {"existElement", alibrary_Objects_Model_existElement},
  {"emitContentFromPath", alibrary_Objects_Model_emitContentFromPath},
  {"addElementText", alibrary_Objects_Model_addElementText},
  {"setElementText", alibrary_Objects_Model_setElementText},
  {NULL, NULL}
};

LUALIB_API int luaopen_model(lua_State *L)
{
  luaL_register(L, "model", model_funcs);
  return 1;
}
