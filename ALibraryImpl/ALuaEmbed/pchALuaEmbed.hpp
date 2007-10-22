#pragma once

#include "apiALuaEmbed.hpp"

#include "AString.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"
#include "AFile.hpp"
#include "templateAutoPtr.hpp"

extern "C"
{
#include "lua.h"

/* Utility functions used by ALibrary */
LUA_API void (luaA_stringappender)(lua_State *L, const char *s);   /* appends a string (const char *) to ALuaEmbed output buffer, now also used by print() */

#include "lauxlib.h"
#include "lualib.h"
#include "lstate.h"

/* ALibrary functionality */
LUALIB_API int (luaopen_alibrary)(lua_State *L);    // register ALibrary generic functions with Lua
LUALIB_API int (luaopen_web)(lua_State *L);         // register ALibrary Web specific functions with Lua

} //extern "C"
