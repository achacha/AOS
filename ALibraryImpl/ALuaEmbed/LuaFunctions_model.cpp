/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AXmlDocument.hpp"
#include "ATemplateContext.hpp"

/*!
Uses the model to emit content for a path

thisfunction("<path to the AXmlElement to emit>", separate)

lua namespace: model
lua param: Path to emit
lua param: if non-nil then each element found will be returned by itself
lua return: Content at the given path or nil if element does not exist
*/
static int alibrary_Objects_Model_emitContentFromPath(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  int mode = 0;
  if (lua_gettop(L) > 1)
    mode = luaL_checkint(L, 2);

  AXmlElement::CONST_CONTAINER nodes;
  size_t ret = pLuaContext->useModel().useRoot().find(xmlpath, nodes);

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

lua namespace: model
lua param: Path to element
lua return: true if exists, false otherwise
*/
static int alibrary_Objects_Model_existElement(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  if (pLuaContext->useModel().useRoot().exists(xmlpath))
    lua_pushboolean(L, 1);
  else
    lua_pushboolean(L, 0);

  return 1;
}

/*!
Emits the entire model as XML

thisfunction([indent = -1])

lua namespace: model
lua param: (Optional) if 0 or greater, will indent the output.  Default -1 will inline it.
lua return: XML string of default AXmlDocument model
*/
static int alibrary_Objects_Model_emitXml(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
  }

  AString str;
  pLuaContext->useModel().useRoot().emit(str, indent);

  lua_pushlstring(L, str.c_str(), str.getSize());
  return 1;
}

/*!
Emits the entire model as JSON

thisfunction([indent = -1])

lua namespace: model
lua param: (Optional) if 0 or greater, will indent the output.  Default -1 will inline it.
lua return: JSON string of default AXmlDocument model
*/
static int alibrary_Objects_Model_emitJson(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  int indent = -1;
  if (lua_gettop(L) > 0)
  {
    indent = luaL_checkint(L, 1);
  }

  AString str;
  pLuaContext->useModel().useRoot().emitJson(str, indent);

  lua_pushlstring(L, str.c_str(), str.getSize());
  return 1;
}

/*!
Adds a new element and optionally a text value to it

thisfunction("element path", "value")

lua param: Element path
lua param: (Optional) Value to set
lua return: nil
*/
static int alibrary_Objects_Model_addElement(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  AXmlElement &e = pLuaContext->useModel().useRoot().addElement(xmlpath);
  if (lua_gettop(L) > 1)
  {
    s = luaL_checklstring(L, 2, &len);
    const AString& value = AString::wrap(s, len);
    e.addData(value);
  }
  return 0;
}

/*!
Adds a text value to an existing element at the given path or will create element

thisfunction("element path", "value")

lua param: Element path
lua param: Value to set
lua return: nil
*/
static int alibrary_Objects_Model_addText(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);

  AXmlElement &e = pLuaContext->useModel().useRoot().overwriteElement(xmlpath);
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

lua param: Element path
lua param: Value to set, if not specified only element is created
lua return: nil
*/
static int alibrary_Objects_Model_setText(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  
  AXmlElement &e = pLuaContext->useModel().useRoot().overwriteElement(xmlpath);
  if (lua_gettop(L) > 1)
  {
    s = luaL_checklstring(L, 2, &len);
    const AString& value = AString::wrap(s, len);

    e.setData(value);
  }

  return 0;
}

/*!
Gets text value for a given path

thisfunction("element path")

lua param: Element path
lua return: Text content for a given element or nil if it doesn't exist
*/
static int alibrary_Objects_Model_getText(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& xmlpath = AString::wrap(s, len);
  
  AXmlElement *pElement = pLuaContext->useModel().useRoot().findElement(xmlpath);
  if (!pElement)
  {
    return 0;
  }
  else
  {
    AString str;
    pElement->emitContent(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
}

static const luaL_Reg model_funcs[] = {
  {"emitXml", alibrary_Objects_Model_emitXml},
  {"emitJson", alibrary_Objects_Model_emitJson},
  {"existElement", alibrary_Objects_Model_existElement},
  {"emitContentFromPath", alibrary_Objects_Model_emitContentFromPath},
  {"addElement", alibrary_Objects_Model_addElement},
  {"addText", alibrary_Objects_Model_addText},
  {"setText", alibrary_Objects_Model_setText},
  {"getText", alibrary_Objects_Model_getText},
  {NULL, NULL}
};

LUALIB_API int luaopen_model(lua_State *L)
{
  luaL_register(L, "model", model_funcs);
  return 1;
}
