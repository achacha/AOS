#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AFile_Socket.hpp"

/*!
Request from web via HTTP

web.HttpGet("http://www.achacha.org/");

@namespace web
@param (string) URL
@return (int) Status code
@return (string) HTTP header
@return (string) HTTP document
*/
static int web_HttpGet(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& url = AString::wrap(s, len);
  lua_pop(L,1);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AHTTPRequestHeader request;
  AHTTPResponseHeader response;

  request.parseUrl(url);
  AFile_Socket httpSocket(request, true);
  httpSocket.open();
  request.toAFile(httpSocket);
  response.fromAFile(httpSocket);

  //a_Return status code
  lua_pushinteger(L, response.getStatusCode());

  //a_Return HTTP response header
  AString str(10240, 8188);  
  response.emit(str);
  lua_pushlstring(L, str.c_str(), str.getSize());
  str.clear();

  //a_Return HTTP response document
  httpSocket.emit(str);
  lua_pushlstring(L, str.c_str(), str.getSize());

  return 2;
}

/*!
Request from web via HTTP

web.HttpGet("http://www.achacha.org/");

@namespace web
@param (string) URL
@return (int) Status code
*/
static int web_HttpGetStatusCode(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& url = AString::wrap(s, len);
  lua_pop(L,1);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AHTTPRequestHeader request;
  AHTTPResponseHeader response;

  request.parseUrl(url);
  AFile_Socket httpSocket(request, true);
  httpSocket.open();
  request.toAFile(httpSocket);
  response.fromAFile(httpSocket);

  //a_Return status code
  lua_pushinteger(L, response.getStatusCode());

  return 1;
}

static const luaL_Reg web_funcs[] = {
  {"getHttp", web_HttpGet},
  {"getHttpStatusCode", web_HttpGetStatusCode},
  {NULL, NULL}
};

LUALIB_API int luaopen_web(lua_State *L)
{
  luaL_register(L, "web", web_funcs);
  return 1;
}
