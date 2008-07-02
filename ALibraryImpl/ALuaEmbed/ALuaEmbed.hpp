/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ALuaEmbed_HPP__
#define INCLUDED__ALuaEmbed_HPP__

#include "apiALuaEmbed.hpp"
#include "ADebugDumpable.hpp"
#include "ATemplateContext.hpp"

class AOutputBuffer;
class AEmittable;

/*!
Lua interpreter

NOTE: objects provided MUST include the AXmlDocument model at ATemplate::OBJECTNAME_MODEL


Usage example
-------------
int somefunction()
{
  // model with just <root>HelloWorld</root>
  AXmlDocument model("root");
  model.addData("HelloWorld");


  // objects
  ABasePtrContainer objects;
  
  // create context
  ATemplateContext tctx(objects, model);

  // Some code
  AString code("print(model.getText('/root/child'));");

  // output
  AString output;
  
  // lua interpreter
  ALuaEmbed lua;
  if (!lua.execute(code, context, output))
    std::cerr << "Error occured: " << output << std::endl;

  // Display results
  std::cout << output << std::endl;
}

When this function goes out of scope objects destructor will delete pOutput but not the model.

*/
class ALuaEMBED_API ALuaEmbed : public ADebugDumpable
{
public:
  /*!
  Binary map of libraries to load
  */
  enum LuaLibrary
  {
    LUALIB_NONE     = 0x0000,       // Mask to load no libraries
    LUALIB_BASE     = 0x0001,       // print() modified to write to ALuaEmbed output buffer
    LUALIB_TABLE    = 0x0002,
    LUALIB_STRING   = 0x0004,
    LUALIB_MATH     = 0x0008,
    LUALIB_DEBUG    = 0x0010,
    //LUALIB_OS       = 0x0020,     // Not supported (security)
    //LUALIB_PACKAGE  = 0x0100,     // Not supported (security)
    //LUALIB_IO       = 0x0200,     // Not supported (security)
    LUALIB_ALL_SAFE = 0x000f,       // All web safe libraries (no OS access)
    LUALIB_ALL      = 0xffff        // Mask to load all libraries
  };

  typedef int (LUA_OPENLIBRARY_FUNCTION)(lua_State *);
  typedef LUA_OPENLIBRARY_FUNCTION* LUA_OPENLIBRARY_FPTR;

public:
  /*!
  Initialize Lua interpreter
  Load libraries into this instance
    Can use LUALIB_CORE|LUALIB_MATH|etc to load some
  */
  ALuaEmbed(u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_BASE);
  
  /*!
  De-initialize and unload the Lua interpreter
  */
  virtual ~ALuaEmbed();

  /*!
  Load user defined library

  Usage example:
    static const luaL_Reg userdefined_funcs[] = {
      {"fname", fname_function},
      {NULL, NULL}
    };

    LUALIB_API int luaopen_userdefined(lua_State *L)
    {
      luaL_register(L, "user", userdefined_funcs);
      return 1;
    }

  After registering you can use: user.fname() in Lua
  */
  void loadUserLibrary(LUA_OPENLIBRARY_FPTR);
  
  /*!
  Execute Lua code
  Use the provided objects and output (see top for example)
  
  @param code to execute
  @param context for the execution
  @param output buffer

  @return true if all is well, false if error occured (written to outputbuffer)
  */
  bool execute(const AEmittable& code, ATemplateContext& context, AOutputBuffer& output);

  /*!
  Lua panic function
  Throws AException instead of calling exit()
  */
  static int callbackPanic(lua_State *);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_The Lua state pointer
  struct lua_State *mp_LuaState;

  //a_Init Lua interpreter and load libraries
  void _init(u4 maskLibrariesToLoad);
};

#endif //INCLUDED__ALuaEmbed_HPP__
