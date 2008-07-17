/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "ARope.hpp"
#include "ATemplate.hpp"

void ALuaEmbed::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_LuaState=" << AString::fromPointer(mp_LuaState) << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ALuaEmbed::ALuaEmbed(
  u4 maskLibrariesToLoad
):
  mp_LuaState(NULL)
{
  //a_Initialize Lua
  _init(maskLibrariesToLoad);
}

ALuaEmbed::~ALuaEmbed()
{
  try
  {
    if (mp_LuaState)
      lua_close(mp_LuaState);
  }
  catch(...) {}
}

void ALuaEmbed::_init(u4 maskLibrariesToLoad)
{
  mp_LuaState = lua_open();
  AASSERT(NULL, mp_LuaState);
  lua_atpanic(mp_LuaState, ALuaEmbed::callbackPanic);

  //a_Explicit NONE selected, no libraries loaded
  if (LUALIB_NONE == maskLibrariesToLoad)
    return;

  //a_Load ALibrary based functions
  luaopen_alibrary(mp_LuaState);         // load ALibrary functions first (and override print() to use output)
  luaopen_base(mp_LuaState);             // load base Lua functions used by ALibrary
  luaopen_web(mp_LuaState);              // load web functions
  luaopen_objects(mp_LuaState);          // load objects functions
  luaopen_model(mp_LuaState);            // load model functions

  //a_Optional Lua libraries
  if (LUALIB_TABLE & maskLibrariesToLoad)   luaopen_table(mp_LuaState);            // opens the table library
  if (LUALIB_STRING & maskLibrariesToLoad)  luaopen_string(mp_LuaState);           // opens the string lib
  if (LUALIB_MATH & maskLibrariesToLoad)    luaopen_math(mp_LuaState);             // opens the math lib
  if (LUALIB_DEBUG & maskLibrariesToLoad)   luaopen_debug(mp_LuaState);            // opens the debug lib

//a_Following are not supported due to security concerns while being embedded, but can be uncommented if needed at your own risk  
//  if (LUALIB_OS & maskLibrariesToLoad)      luaopen_os(mp_LuaState);               // opens the OS lib
//  if (LUALIB_PACKAGE & maskLibrariesToLoad) luaopen_package(mp_LuaState);          // opens the package lib
//  if (LUALIB_IO & maskLibrariesToLoad)      luaopen_io(mp_LuaState);               // opens the I/O library

  lua_gc(mp_LuaState, LUA_GCSETPAUSE, 120);
  lua_gc(mp_LuaState, LUA_GCSETSTEPMUL, 400); 
}

void ALuaEmbed::loadUserLibrary(LUA_OPENLIBRARY_FPTR fptr)
{
  fptr(mp_LuaState);
}

int ALuaEmbed::callbackPanic(lua_State *L)
{
  //a_We are here because Lua has paniced
  ATHROW_EX(NULL, AException::OperationFailed, ASWNL("Lua has had a panic attack."));
}

bool ALuaEmbed::execute(const AEmittable& code, ATemplateContext& context, AOutputBuffer& output)
{
  AASSERT(NULL, mp_LuaState);
  bool result = true;
  try
  {
    //a_Assign current context to lua state
    mp_LuaState->acontext = &context;
    mp_LuaState->aoutputbuffer = &output;

    ARope rope;
    code.emit(rope);
    const AString& buffer = rope.toAString();
    int error = luaL_loadbuffer(mp_LuaState, buffer.c_str(), buffer.getSize(), "buffer");
    if (error)
    {
      AString strError(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      output.append(strError);
      result = false;
    }
    else
    {
      //a_Execute LUA code
      switch(lua_pcall(mp_LuaState, 0, LUA_MULTRET, 0))
      {
        case LUA_ERRRUN:
        {
          AString strError("Runtime Error: ");
          strError.append(lua_tostring(mp_LuaState, -1));
          lua_pop(mp_LuaState, 1);  // pop error message from the stack
          output.append(strError);
          result = false;
        } 
        break;
        
        case LUA_ERRMEM:
        {
          AString strError("Memory Allocation Error: ");
          strError.append(lua_tostring(mp_LuaState, -1));
          lua_pop(mp_LuaState, 1);  // pop error message from the stack
          output.append(strError);
          result = false;
        } 
        break;

        case LUA_ERRERR:
        {
          AString strError("Error in error handler: ");
          strError.append(lua_tostring(mp_LuaState, -1));
          lua_pop(mp_LuaState, 1);  // pop error message from the stack
          output.append(strError);
          result = false;
        }
        break;
      }
    }
    mp_LuaState->acontext = NULL;
    mp_LuaState->aoutputbuffer = NULL;
  }
  catch(AException& ex)
  {
    ex.emit(output);
    mp_LuaState->acontext = NULL;
    mp_LuaState->aoutputbuffer = NULL;
    result = false;
  }
  return result;
}
