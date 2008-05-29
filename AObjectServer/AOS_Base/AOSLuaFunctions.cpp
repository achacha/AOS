/*
Written by Alex Chachanashvili

$Id$
*/
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
#include "ATemplateContext.hpp"

/*!
Gets HTTP request header parameter

Special names:
_METHOD  - HTTP request method in line 0
_URL     - URL in line 0
_VERSION - HTTP version in line 0
_ALL     - Entire HTTP header (without last CRLF)

Given:
GET /mypath/myprog?a=2 HTTP/1.1
Host: www.someserver.com

Example:
aos.getRequestHeader("Host");    -->  www.someserver.com
aos.getRequestHeader("DNE");     -->  nil
aos.getRequestHeader("_METHOD")  --> GET
aos.getRequestHeader("_URL")     --> /mypath/myprog?a=2

@namespace aos
@param HTTP request header name
@return HTTP request header value or nil if name does not exist
*/
static int aos_getRequestHeader(lua_State *L)
{
  static AString METHOD("_METHOD");
  static AString URL("_URL");
  static AString VERSION("_VERSION");
  static AString ALL("_ALL");
  
  //a_Get reference to ALuaEmbed object
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaContext->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& name = AString::wrap(s, len);

  if (name.equals(METHOD))
  {
    const AString& str = pContext->useRequestHeader().getMethod();
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else if (name.equals(URL))
  {
    AString str;
    pContext->useRequestUrl().emit(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else if (name.equals(VERSION))
  {
    const AString& str = pContext->useRequestHeader().getVersion();
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else if (name.equals(ALL))
  {
    AString str(2048, 2048);
    pContext->useRequestHeader().emit(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else
  {
    AString str;
    if (pContext->useRequestHeader().getPairValue(name, str))
    {
      lua_pushlstring(L, str.c_str(), str.getSize());
      return 1;
    }
  }
  return 0;
}

/*!
Gets HTTP request cookie value from the header

Example:
aos.getRequestCookie("username");
Returns:
foo  (if request header had parameter:  Cookie: username=foo)

@namespace aos
@param Request cookie name
@return Cookie value or nil if name does not exist
*/
static int aos_getRequestCookie(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaContext->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& name = AString::wrap(s, len);

  AString str;
  if (pContext->useRequestCookies().getValue(name, str))
  {
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  return 0;
}

/*!
Gets HTTP request parameter value from the header
  (may contain form data parameters also if using form input processor)
  (may return more than one value)

Given:
GET /someurl?foo=1&foo=2&foo=3&bar=1 HTTP/1.0
...

Example:
aos.getRequestParameter("foo");  -->  "1", "2", "3"
aos.getRequestParameter("bar");  -->  "1"
aos.getRequestParameter("baz");  -->  nil

@namespace aos
@param Request parameter name
@return Parameter values (1 or more) or nil if name does not exist
*/
static int aos_getRequestParameter(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaContext->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& name = AString::wrap(s, len);

  LIST_NVPair values;
  size_t count = pContext->useRequestParameterPairs().get(name, values);
  if (count > 0)
  {
    for (LIST_NVPair::iterator it = values.begin(); it != values.end(); ++it)
    {
      const AString& str = it->getValue();
      lua_pushlstring(L, str.c_str(), str.getSize());
    }
    return (int)count;
  }
  return 0;
}

/*!
Gets HTTP request parameter names from query string or form submit

Given:
GET /someurl?foo=1&foo=2&foo=3&bar=1 HTTP/1.0
...

Example:
aos.getRequestParameterNames();  -->  "foo", "bar"

@namespace aos
@param name of the parameter
@return HTTP request header value or nil if name does not exist
*/
static int aos_getRequestParameterNames(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaContext->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  SET_AString names;
  pContext->useRequestParameterPairs().getNames(names);
  for (SET_AString::iterator it = names.begin(); it != names.end(); ++it)
    lua_pushlstring(L, it->c_str(), it->getSize());
  return names.size();
}

/*!
Sets HTTP response header parameter
Does not guarantee that is may not be overwritten by the server
  (if you set output content type here, the server will change it to account for the actual content)
  (most server headers are appended, but Content-Type and Content-Length are known to be overwritten)

Example:
aos.setResponseHeader("Set-Cookie", "name=foo; max-ago=1000;");

@namespace aos
@param name of the response header to set
@param value of respponse header to set
@return nil
*/
static int aos_setResponseHeader(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaContext->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: xmlpath
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& name = AString::wrap(s, len);

  //a_param 1: text
  s = luaL_checklstring(L, 2, &len);
  const AString& value = AString::wrap(s, len);

  pContext->useResponseHeader().setPair(name, value);
  return 0;
}

/*!
Sets event to the current in event visitor object
(old event is placed into event queue, which is used to follow the execution of the context)
(errors are flagged as such and may result in error page (such as 500))

Example:
aos.addEvent("Hello from Lua!");      --> Sets current event
aos.addEvent("Error from Lua!", 1);   --> Will cause error 500

@namespace aos
@param event data to set as current in event visitor
@param nil or zero if a message, non-zero if an error
@return nil
*/
static int aos_setEvent(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaContext->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  AASSERT_EX(NULL, lua_gettop(L) >= 1, ASWNL("Not enough actual parameters, expected at least 1"));

  //a_param 0: event data
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& data = AString::wrap(s, len);

  int isError = 0;
  if (lua_gettop(L) > 1)
  {
    isError = luaL_checkint(L, 2);
  }

  //a_Add Event
  if (!data.isEmpty())
    pContext->useEventVisitor().startEvent(data, (isError ? AEventVisitor::EL_ERROR : AEventVisitor::EL_EVENT));

  return 0;
}

/*!
Resets event and stops current event timer
(Current event is moved into the old event queue and its timer is stopped)
(This is used to time events)

Example:
aos.setEvent("Starting something");

-- do stuff --

aos.resetEvent();      --> Sets current event to nothing, moves current into old event queue and stops current event timer

@namespace aos
@return nil
*/
static int aos_resetEvent(lua_State *L)
{
  //a_Get reference to ALuaEmbed object
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  //a_Get AOSContext stored in /context
  AOSContext *pContext = pLuaContext->useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  AASSERT(NULL, pContext);

  pContext->useEventVisitor().endEvent();

  return 0;
}

static const luaL_Reg aos_funcs[] = {
  {"getRequestHeader", aos_getRequestHeader},
  {"getRequestCookie", aos_getRequestCookie},
  {"getRequestParameter", aos_getRequestParameter},
  {"getRequestParameterNames", aos_getRequestParameterNames},
  {"setResponseHeader", aos_setResponseHeader},
  {"setEvent", aos_setEvent},
  {"resetEvent", aos_resetEvent},
  {NULL, NULL}
};

extern "C" int luaopen_aos(lua_State *L)
{
  luaL_register(L, "aos", aos_funcs);
  return 1;
}
