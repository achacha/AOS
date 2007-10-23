#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"

/*!
Looks up an object as AEmittable and emits it

thisfunction("<object name of AEmittable type>")

@namespace objects
@param Object name
@return Emitted object output
*/
static int alibrary_Objects_emit(lua_State *L)
{
  size_t len = AConstant::npos;
  const char *s = luaL_checklstring(L, 1, &len);
  const AString& objectName = AString::wrap(s, len);
  lua_pop(L,1);

  ALuaEmbed *pLuaEmbed = (ALuaEmbed *)(L->mythis);
  AASSERT(NULL, pLuaEmbed);

  AEmittable *p = pLuaEmbed->useObjects().useAsPtr<AEmittable>(objectName);
  if (p)
  {
    AString str;
    p->emit(str);
    lua_pushlstring(L, str.c_str(), str.getSize());
    return 1;
  }
  else
    return 0;
}

static const luaL_Reg objects_funcs[] = {
  {"emit", alibrary_Objects_emit},
  {NULL, NULL}
};

LUALIB_API int luaopen_objects(lua_State *L)
{
  luaL_register(L, "objects", objects_funcs);
  return 1;
}
