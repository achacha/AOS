#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AFile_Socket.hpp"

/*!
Request from web via HTTP

web.HttpGet("http://www.achacha.org/");

Returns:
  HTTP header
  HTTP document

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

  //a_Return HTTP response header
  AString str(10240, 8188);  
  response.emit(str);
  lua_pushlstring(L, str.c_str(), str.getSize());
  str.clear();

  //a_Return HTTP response document
  httpSocket.emit(str);
  lua_pushlstring(L, str.c_str(), str.getSize());
  str.clear();

  return 2;
}

static const luaL_Reg web_funcs[] = {
  {"HttpGet", web_HttpGet},
  {NULL, NULL}
};

LUALIB_API int luaopen_web(lua_State *L)
{
  luaL_register(L, "web", web_funcs);
  return 1;
}
