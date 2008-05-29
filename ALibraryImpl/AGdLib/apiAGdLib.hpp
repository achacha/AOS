/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiAGdLib_HPP__
#define INCLUDED__apiAGdLib_HPP__

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef AGDLIB_EXPORTS
#      pragma message("AGdLib: EXPORT")
#      define AGDLIB_API __declspec(dllexport)
#      pragma comment(lib, "bgd")
#    else
#      pragma message("AGdLib: IMPORT")
#      define AGDLIB_API __declspec(dllimport)
#      pragma comment(lib, "AGdLib")
#    endif
#  else
#    pragma message("AGdLib: NONE")
#    define AGDLIB_API
#  endif
#endif

#pragma comment(lib, "bgd")

#include "apiABase.hpp"
//#include "config.h"
#include "gd.h"

#endif //INCLUDED__apiAGdLib_HPP__
