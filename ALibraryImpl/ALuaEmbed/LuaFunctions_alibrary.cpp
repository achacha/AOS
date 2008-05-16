#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AThread.hpp"
#include "ATemplateContext.hpp"

/*!
Appender used to overwrite print() and write to output buffer
*/
extern "C" void luaL_stringappender(lua_State *L, const char *s)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext);
  AASSERT(NULL, pLuaContext);
  if (pLuaContext)
  {
    pLuaContext->useOutput().append(s);
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
  u4 sleeptime = (u4)luaL_checkint(L, 1);
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
