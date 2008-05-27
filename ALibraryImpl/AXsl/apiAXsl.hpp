/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__apiAXsl_HPP__
#define INCLUDED__apiAXsl_HPP__

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef AXSL_EXPORTS
#      pragma message("AXsl: EXPORT")
#      define AXSL_API __declspec(dllexport)
#    else
#      pragma message("AXsl: IMPORT")
#      define AXSL_API __declspec(dllimport)
#      pragma comment(lib, "AXsl")
#    endif
#  else
#    pragma message("AXsl: NONE")
#    define AXSL_API
#  endif
#endif

#include "apiABase.hpp"

#endif //INCLUDED__apiAXsl_HPP__
