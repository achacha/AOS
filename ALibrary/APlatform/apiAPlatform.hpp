/*
Written by Alex Chachanashvili

$Id: apiAPlatform.hpp 328 2010-08-24 21:01:36Z achacha $
*/
#ifndef INCLUDED__apiAPlatform_HPP__
#define INCLUDED__apiAPlatform_HPP__

//a_OS Defines and compiler specifics
#include "apiABase.hpp"

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
//a_Static libraries (_LIB) do not need explicit export
//a_export linkage for current dynamic object
//a_import linkage for all others
//a_no linkage for isolation
#if defined(_MSC_VER) && defined(__WINDOWS__)
#  ifdef APLATFORM_EXPORTS
#    pragma message("APlatform: EXPORT")
#    define APLATFORM_API __declspec(dllexport)
#    pragma comment(lib, "kernel32")
#    pragma comment(lib, "user32")
#    pragma comment(lib, "advapi32")
#    pragma comment(lib, "wsock32")
#    pragma comment(lib, "ws2_32")
#    pragma comment(lib, "iphlpapi")
#  else
#    pragma message("APlatform: IMPORT")
#    define APLATFORM_API __declspec(dllimport)
#    pragma comment(lib, "APlatform")
#  endif
#else
#define APlatform_API
#endif

//a_Version
#define APlatform_INFO "APlatform 1.0.0 " ## __BUILDTYPE__ ## " Build(" ## __TIMESTAMP__ ## ") for " ## APlatform_OS_INFO

#endif //INCLUDED__apiAPlatform_HPP__
