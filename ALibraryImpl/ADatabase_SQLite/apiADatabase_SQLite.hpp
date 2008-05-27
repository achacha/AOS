/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__apiADatabase_SQLite_HPP__
#define INCLUDED__apiADatabase_SQLite_HPP__

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef ADATABASE_SQLITE_EXPORTS
#      pragma message("ADatabase_SQLite: EXPORT")
#      define ADATABASE_SQLITE_API __declspec(dllexport)
#    else
#      pragma message("ADatabase_SQLite: IMPORT")
#      define ADATABASE_SQLITE_API __declspec(dllimport)
#      pragma comment(lib, "ADatabase_SQLite")
#    endif
#  else
#    pragma message("ADatabase_SQLite: NONE")
#    define ADATABASE_SQLITE_API
#  endif
#else
#define ADATABASE_SQLITE_API
#endif

#include "apiABase.hpp"
#include "sqlite3.h"

#endif //INCLUDED__apiADatabase_SQLite_HPP__
