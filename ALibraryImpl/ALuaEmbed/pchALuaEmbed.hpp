/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "apiALuaEmbed.hpp"

#include "AString.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"
#include "AFile.hpp"
#include "templateAutoPtr.hpp"
#include "LuaInternals.hpp"

extern "C"
{
/* ALibrary functionality */
LUALIB_API int (luaopen_alibrary)(lua_State *L);    // register ALibrary generic functions with Lua
LUALIB_API int (luaopen_web)(lua_State *L);         // register ALibrary Web specific functions with Lua
LUALIB_API int (luaopen_objects)(lua_State *L);         // register ALibrary Web specific functions with Lua
LUALIB_API int (luaopen_model)(lua_State *L);         // register ALibrary Web specific functions with Lua

} //extern "C"
