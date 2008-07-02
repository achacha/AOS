/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiALuaEmbed_HPP__
#define INCLUDED__apiALuaEmbed_HPP__

#ifdef ALuaEMBED_EXPORTS
#  pragma message("ALuaEmbed: EXPORT")
#  define ALuaEMBED_API __declspec(dllexport)
#  ifndef LUA_BUILD_AS_DLL
#  define LUA_BUILD_AS_DLL
#  endif
#  ifndef LUA_LIB
#  define LUA_LIB
#  endif
#else
#  pragma message("ALuaEmbed: IMPORT")
#  define ALuaEMBED_API __declspec(dllimport)
#  pragma comment(lib, "ALuaEmbed")
#  define LUA_BUILD_AS_DLL
#  define LUA_LIB
#endif

#include "apiABase.hpp"
#include "LuaInternals.hpp"

#define ALuaEmbed_INFO LUA_RELEASE

#endif //INCLUDED__apiALuaEmbed_HPP__