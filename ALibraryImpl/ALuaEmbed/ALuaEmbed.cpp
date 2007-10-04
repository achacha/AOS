// ALuaEmbed.cpp : Defines the entry point for the DLL application.
//

#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "ALibraryFunctions.hpp"
extern "C"
{
#include "lstate.h"
}

ALuaEmbed::ALuaEmbed(u4 maskLibrariesToLoad):
  mp_LuaState(NULL)
{
  mp_LuaState = lua_open();
  AASSERT(NULL, mp_LuaState);
  mp_LuaState->mythis = this;  //a_Attach out output buffer

  //a_Explicit NONE selected, no libraries loaded
  if (LUALIB_NONE == maskLibrariesToLoad)
    return;

  //a_Load ALibrary functions first
  luaopen_alibrary(mp_LuaState);
  luaopen_base(mp_LuaState);             // opens the basic library required by ALibrary

  if (LUALIB_TABLE & maskLibrariesToLoad)   luaopen_table(mp_LuaState);            // opens the table library
  if (LUALIB_STRING & maskLibrariesToLoad)  luaopen_string(mp_LuaState);           // opens the string lib
  if (LUALIB_MATH & maskLibrariesToLoad)    luaopen_math(mp_LuaState);             // opens the math lib
  if (LUALIB_DEBUG & maskLibrariesToLoad)   luaopen_debug(mp_LuaState);            // opens the debug lib

//a_Following are not supported due to security concerns while being embedded, but can be uncommented if needed at your own risk  
//  if (LUALIB_OS & maskLibrariesToLoad)      luaopen_os(mp_LuaState);               // opens the OS lib
//  if (LUALIB_PACKAGE & maskLibrariesToLoad) luaopen_package(mp_LuaState);          // opens the package lib
//  if (LUALIB_IO & maskLibrariesToLoad)      luaopen_io(mp_LuaState);               // opens the I/O library
}

ALuaEmbed::~ALuaEmbed()
{
  if (mp_LuaState)
    lua_close(mp_LuaState);
}

bool ALuaEmbed::execute(const AEmittable& code, AOutputBuffer& scriptError)
{
  AASSERT(NULL, mp_LuaState);
  ARope rope;
  code.emit(rope);
  const AString& buffer = rope.toAString();
  int error = luaL_loadbuffer(mp_LuaState, buffer.c_str(), buffer.getSize(), "buffer");
  if (error)
  {
    AString strError(lua_tostring(mp_LuaState, -1));
    lua_pop(mp_LuaState, 1);  // pop error message from the stack
    scriptError.append(strError);
    return false;
  }

  //a_Execute LUA code
  switch(lua_pcall(mp_LuaState, 0, LUA_MULTRET, 0))
  {
    case LUA_ERRRUN:
    {
      AString strError("Runtime Error: ");
      strError.append(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      scriptError.append(strError);
      return false;
    } 
    break;
    
    case LUA_ERRMEM:
    {
      AString strError("Memory Allocation Error: ");
      strError.append(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      scriptError.append(strError);
      return false;
    } 
    break;

    case LUA_ERRERR:
    {
      AString strError("Error in error handler: ");
      strError.append(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      scriptError.append(strError);
      return false;
    }
    break;
  }
}

AOutputBuffer& ALuaEmbed::useOutputBuffer()
{
  return m_OutputBuffer;
}

AObjectContainer& ALuaEmbed::useObjects()
{
  return m_Objects;
}

