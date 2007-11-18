#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AThread.hpp"

/*!
Appender used to overwrite print() and write to output buffer
*/
extern "C" void luaL_stringappender(lua_State *L, const char *s)
{
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);
  if (pLuaEmbed)
  {
    pLuaEmbed->useOutput().append(s);
  }
}

/*!
Sleeps for N milliseconds

@namespace alibrary
@param (integer) Milliseconds
@return nil
*/
static int alibrary_Sleep(lua_State *L)
{
  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  size_t len = AConstant::npos;
  u4 sleeptime = (u4)luaL_checkint(L, 1, &len);

  AThread::sleep(sleeptime);

  return 0;
}

static const luaL_Reg alibrary_funcs[] = {
  {"sleep", alibrary_Sleep},
  {NULL, NULL}
};

LUALIB_API int luaopen_alibrary(lua_State *L)
{
  luaL_register(L, "alibrary", alibrary_funcs);
  return 1;
}
