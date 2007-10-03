#ifndef INCLUDED__apiALuaEmbed_HPP__
#define INCLUDED__apiALuaEmbed_HPP__

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ALuaEMBED_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ALuaEMBED_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ALuaEMBED_EXPORTS
#  pragma message("ALuaEmbed: EXPORT")
#  define ALuaEMBED_API __declspec(dllexport)
#  define LUA_BUILD_AS_DLL
#  define LUA_LIB
#else
#  pragma message("ALuaEmbed: IMPORT")
#  define ALuaEMBED_API __declspec(dllimport)
#  pragma comment(lib, "ALuaEmbed")
#endif

#include "apiABase.hpp"

#endif //INCLUDED__apiALuaEmbed_HPP__