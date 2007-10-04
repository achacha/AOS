/* This header is inside extern "C" wrapper */
#ifndef INCLUDED__ALibraryFunctions_hpp__
#define INCLUDED__ALibraryFunctions_hpp__

extern "C"
{
#include "lua.h"

/* ALibrary functionality */
#define LUA_ALIBRARYNAME "alibrary"
LUALIB_API int (luaopen_alibrary)(lua_State *L);                  /* register ALibrary functions with Lua */

/* Utility functions used by ALibrary */
LUA_API void (luaA_stringappender)(lua_State *L, const char *s);   /* appends a string (const char *) to ALuaEmbed output buffer, now also used by print() */

#include "lauxlib.h"
#include "lualib.h"

} //extern "C"

#endif //INCLUDED__ALibraryFunctions_hpp__
