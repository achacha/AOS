/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiADatabase_ODBC_HPP__
#define INCLUDED__apiADatabase_ODBC_HPP__

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef ADATABASE_ODBC_EXPORTS
#      pragma message("ADatabase_ODBC: EXPORT")
#      define ADATABASE_ODBC_API __declspec(dllexport)
#      pragma comment(lib, "odbc32")
#      pragma comment(lib, "odbccp32")
#    else
#      pragma message("ADatabase_ODBC: IMPORT")
#      define ADATABASE_ODBC_API __declspec(dllimport)
#      pragma comment(lib, "ADatabase_ODBC")
#    endif
#  else
#    pragma message("ADatabase_ODBC: NONE")
#    define ADATABASE_ODBC_API
#  endif
#else
#define ADATABASE_ODBC_API
#endif

#include "apiABase.hpp"

#ifdef __WINDOWS__
#include <sqlext.h>
#define ADatabase_ODBC_INFO "ADatabase_ODBC " ## __BUILDTYPE__ ## " Build(" ## __TIMESTAMP__ ## ") using Microsoft ODBC " ## SQL_SPEC_STRING
#else
#define ADatabase_ODBC_INFO "ODBC not available"
#endif

#endif //INCLUDED__apiADatabase_ODBC_HPP__
