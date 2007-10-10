#ifndef INCLUDED__ALuaEmbed_HPP__
#define INCLUDED__ALuaEmbed_HPP__

#include "apiALuaEmbed.hpp"
#include "ADebugDumpable.hpp"
#include "ABasePtrHolder.hpp"
#include "templateAutoPtr.hpp"
#include "AXmlDocument.hpp"

class AOutputBuffer;
class AEmittable;
struct lua_State;

/*!
Lua interpreter
*/
class ALuaEMBED_API ALuaEmbed : public ADebugDumpable
{
public:
  /*!
  Binary map of libraries to load
  */
  enum LuaLibrary
  {
    LUALIB_NONE    = 0x0000,       // Mask to load no libraries
    LUALIB_BASE    = 0x0001,       // print() modified to write to ALuaEmbed output buffer
    LUALIB_TABLE   = 0x0002,
    LUALIB_STRING  = 0x0004,
    LUALIB_MATH    = 0x0008,
    LUALIB_DEBUG   = 0x0010,
    //LUALIB_OS      = 0x0020,     //Not supported (security)
    //LUALIB_PACKAGE = 0x0100,     //Not supported (security)
    //LUALIB_IO      = 0x0200,     //Not supported (security)
    LUALIB_ALL     = 0xffff        // Mask to load all libraries
  };

  typedef int (LUA_OPENLIBRARY_FUNCTION)(lua_State *);
  typedef LUA_OPENLIBRARY_FUNCTION* LUA_OPENLIBRARY_FPTR;

public:
  /*!
  Initialize Lua interpreter
  Load libraries into this instance
    Can use LUALIB_CORE|LUALIB_MATH|etc to load some
  
  AXmlDocument at ATemplate::OBJECTNAME_MODEL will be added by default with single /root
  */
  ALuaEmbed(
    u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_BASE
  );
  
  /*!
  Initialize Lua interpreter
  Load libraries into this instance
    Can use LUALIB_CORE|LUALIB_MATH|etc to load some

  Configure to use external objects and output buffer

  objects provided MUST include the model AXmlDocument at ATemplate::OBJECTNAME_MODEL
  */
  ALuaEmbed(
    ABasePtrHolder& objects,
    AOutputBuffer& output,
    u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_BASE
  );
  
  /*!
  De-initialize and unload the Lua interpreter
  */
  ~ALuaEmbed();

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
  If you provided objects and output durig creation they will be used
  If default ctor was used then there were defaults created and will be used

  Returns true if all is well, false if error occured (written to outputbuffer)
  */
  bool ALuaEmbed::execute(const AEmittable& code);

  /*!
  Execute Lua code
  Use objects and output provided
  
  Returns true if all is well, false if error occured (written to outputbuffer)
  */
  bool ALuaEmbed::execute(const AEmittable& code, ABasePtrHolder& objects, AOutputBuffer& output);

  /*!
  Output buffer used by lua print command when LUALIB_BASE is loaded
  This can be accssed from Lua via lua_State::outputbuffer
  */
  AOutputBuffer& useOutput();

  /*!
  ABasePtrHolder helper functions that can be called from the python exported ones
  */
  ABasePtrHolder& useObjects();

  /*!
  AXmlDocument at ATemplate::OBJECTNAME_MODEL
  */
  AXmlDocument& useModel();

  /*!
  Lua panic function
  Throws AException instead of calling exit()
  */
  static int callbackPanic(lua_State *);

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

private:
  //a_Simple AString -> ABase map
  AAutoPtr<ABasePtrHolder> mp_Objects;

  //a_Output buffer
  AAutoPtr<AOutputBuffer> mp_Output;

  //a_The Lua state pointer
  struct lua_State *mp_LuaState;

  //a_Init Lua interpreter and load libraries
  void _init(u4 maskLibrariesToLoad);

  //a_Execute Lua code 
  bool _execute(const AEmittable& code);

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ALuaEmbed_HPP__
