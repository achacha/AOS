#ifndef INCLUDED__ALuaEmbed_HPP__
#define INCLUDED__ALuaEmbed_HPP__

#include "apiALuaEmbed.hpp"

class AEmittable;
struct lua_State;

/*!
This object is meant to be global and called as needed
*/
class ALuaEMBED_API ALuaEmbed
{
public:
  /*!
  Initialize Lua interpreter and bind ALibrary methods
  */
  ALuaEmbed();
  
  /*!
  De-initialize and unload the Lua interpreter
  */
  ~ALuaEmbed();

  /*!
  Execute Lua code
  */
  void execute(const AEmittable&);

private:
  lua_State *mp_LuaState;
};

#endif //INCLUDED__ALuaEmbed_HPP__
