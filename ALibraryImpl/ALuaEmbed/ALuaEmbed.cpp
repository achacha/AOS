// ALuaEmbed.cpp : Defines the entry point for the DLL application.
//

#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "ALuaExportFunctions.hpp"

ALuaEmbed::ALuaEmbed():
  mp_LuaState(NULL)
{
  mp_LuaState = lua_open();
}

ALuaEmbed::~ALuaEmbed()
{
  if (mp_LuaState)
    lua_close(mp_LuaState);
}

void ALuaEmbed::execute(const AEmittable& code)
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
    ATHROW_EX(&buffer, AException::APIFailure, strError);
  }
  switch(lua_pcall(mp_LuaState, 0, LUA_MULTRET, 0))
  {
    case LUA_ERRRUN:
    {
      AString strError("Runtime Error: ");
      strError.append(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      ATHROW_EX(&buffer, AException::APIFailure, strError);
    } 
    break;
    
    case LUA_ERRMEM:
    {
      AString strError("Memory Allocation Error: ");
      strError.append(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      ATHROW_EX(&buffer, AException::APIFailure, strError);
    } 
    break;

    case LUA_ERRERR:
    {
      AString strError("Error in error handler: ");
      strError.append(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      ATHROW_EX(&buffer, AException::APIFailure, strError);
    }
    break;
  }
}


