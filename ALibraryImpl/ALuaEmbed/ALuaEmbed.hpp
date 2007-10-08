#ifndef INCLUDED__ALuaEmbed_HPP__
#define INCLUDED__ALuaEmbed_HPP__

#include "apiALuaEmbed.hpp"
#include "ABase.hpp"
#include "ABasePtrHolder.hpp"
#include "templateAutoPtr.hpp"

class AOutputBuffer;
class AEmittable;
struct lua_State;

/*!
Lua interpreter
*/
class ALuaEMBED_API ALuaEmbed : public ABase
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
  
  NOTE: AOutputBuffer pointer is OWNED and DELETED by this class
  If none is specified, this object will create one
  
  If AObjectPtrHolder is specified, it will be used as data
  Else new internal one will be
  "ALuaEmbed" object in the container will point to this class in ctor and removed in dtor

  NOTE: Make sure the AObjectPtrHolder scope outlives this obect (i.e. do not deallocate it until this object is)
  */
  ALuaEmbed(
    u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_BASE
  );
  ALuaEmbed(
    ABasePtrHolder& basePointers,
    u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_BASE
  );
  ALuaEmbed(
    ABasePtrHolder& basePointers,
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
  Returns true if all is well, false if error occured (written to outputbuffer)
  */
  bool execute(const AEmittable& script);

  /*!
  Output buffer used by lua print command when LUALIB_BASE is loaded
  This can be accssed from Lua via lua_State::outputbuffer
  */
  AOutputBuffer& useOutputBuffer();

  /*!
  ABasePtrHolder helper functions that can be called from the python exported ones
  */
  ABasePtrHolder& useBasePtrHolder();

  /*!
  Lua panic function
  Throws AException instead of calling exit()
  */
  static int callbackPanic(lua_State *);

private:
  //a_Simple AString -> AObjectBase map
  //a_If not provided a new one is created
  AAutoPtr<ABasePtrHolder> mp_Objects;

  //a_The Lua state pointer
  struct lua_State *mp_LuaState;

  //a_Output buffer if not provided by creator, one will be allocated
  AAutoPtr<AOutputBuffer> mp_OutputBuffer;

  //a_Init Lua interpreter and load libraries
  void _init(u4 maskLibrariesToLoad);
};

#endif //INCLUDED__ALuaEmbed_HPP__
