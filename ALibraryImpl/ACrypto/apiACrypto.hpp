/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__apiACrypto_HPP__
#define INCLUDED__apiACrypto_HPP__

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef ACRYPTO_EXPORTS
#      pragma message("ACrypto: EXPORT")
#      define ACRYPTO_API __declspec(dllexport)
#      pragma comment(lib, "libeay32")
#      pragma comment(lib, "ssleay32")
#    else
#      pragma message("ACrypto: IMPORT")
#      define ACRYPTO_API __declspec(dllimport)
#      pragma comment(lib, "ACrypto")
#    endif
#  else
#    pragma message("ACrypto: NONE")
#    define ACRYPTO_API
#  endif
#endif

#include "apiABase.hpp"

#endif //INCLUDED__apiACrypto_HPP__
