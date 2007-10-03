#ifndef INCLUDED__ALuaEmbed_HPP__
#define INCLUDED__ALuaEmbed_HPP__

#include "apiALuaEmbed.hpp"
#include "ARope.hpp"

class AEmittable;
struct lua_State;

/*!
This object is meant to be global and called as needed
*/
class ALuaEMBED_API ALuaEmbed
{
public:
  /*!
  Binary map of libraries to load
  */
  enum LuaLibrary
  {
    LUALIB_NONE    = 0x0000,       // Mask to load no libraries
    LUALIB_BASE    = 0x0001,
    LUALIB_TABLE   = 0x0002,
    LUALIB_STRING  = 0x0004,
    LUALIB_MATH    = 0x0008,
    LUALIB_OS      = 0x0010,
    LUALIB_DEBUG   = 0x0020,
    //LUALIB_PACKAGE = 0x0100,     //Not supported
    //LUALIB_IO      = 0x0200,     //Not supported
    LUALIB_ALL     = 0xffff        // Mask to load all libraries
  };

public:
  /*!
  Initialize Lua interpreter
  Load libraries into this instance
    Can use LUALIB_CORE|LUALIB_MATH|etc to load some
  */
  ALuaEmbed(u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_NONE);
  
  /*!
  De-initialize and unload the Lua interpreter
  */
  ~ALuaEmbed();

  /*!
  Execute Lua code
  */
  void execute(const AEmittable&);

  /*!
  Output buffer used by lua print command when LUALIB_BASE is loaded
  This can be accssed from Lua via lua_State::outputbuffer
  */
  AOutputBuffer& useOutputBuffer();

private:
  struct lua_State *mp_LuaState;

  ARope m_OutputBuffer;
};

#endif //INCLUDED__ALuaEmbed_HPP__
