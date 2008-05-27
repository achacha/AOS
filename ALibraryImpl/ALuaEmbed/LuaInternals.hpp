/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef DEFINED__LUAINTERNALS_HPP__
#define DEFINED__LUAINTERNALS_HPP__

/*!
Functions needed by authors of Lua C functions
*/
extern "C"
{
#include "lua.h"

/*!
String appender utility functions used by ALibrary to override print()
Output goes to the string buffer instead of the console
*/
LUA_API void (luaL_stringappender)(lua_State *L, const char *s);   /* appends a string (const char *) to ALuaEmbed output buffer, now also used by print() */

#include "lauxlib.h"
#include "lualib.h"
#include "lstate.h"
}

#endif
