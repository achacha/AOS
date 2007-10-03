#include "pchALuaEmbed.hpp"
#include "ALibraryFunctions.hpp"
#include "ALuaEmbed.hpp"
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

static int foo(lua_State *L)
{
  luaA_stringappender(L, "foo has been called!\r\n");
  return 0;
}

static const luaL_Reg alibrary_funcs[] = {
  {"foo", foo},
  {NULL, NULL}
};

LUALIB_API int luaopen_alibrary(lua_State *L) {
  luaL_register(L, LUA_ALIBRARYNAME, alibrary_funcs);
  return 1;
}
