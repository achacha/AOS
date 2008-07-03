/*
Written by Alex Chachanashvili

$Id$
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

#ifdef WIN32
//a_Include openssl and define open ssl dir to be current dir else it will be a unix path
#define OPENSSLDIR "."
#endif
#include "openssl/opensslv.h"

#define ACrypto_INFO OPENSSL_VERSION_TEXT

#endif //INCLUDED__apiACrypto_HPP__
