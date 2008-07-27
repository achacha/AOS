/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiAZlib_HPP__
#define INCLUDED__apiAZlib_HPP__

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef AZLIB_EXPORTS
#      pragma message("AZlib: EXPORT")
#      define AZLIB_API __declspec(dllexport)
#      ifndef NDEBUG
#        pragma comment(lib, "zlib1d")
#      else
#        pragma comment(lib, "zlib1")
#      endif
#    else
#      pragma message("AZlib: IMPORT")
#      define AZLIB_API __declspec(dllimport)
#      pragma comment(lib, "AZlib")
#    endif
#  else
#    pragma message("AZlib: NONE")
#    define AZLIB_API
#  endif
#endif

#include "apiABase.hpp"

#define AZlib_INFO "zlib 1.2.3"

#endif //INCLUDED__apiAZlib_HPP__
