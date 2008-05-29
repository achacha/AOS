/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiADatabase_MySQL_HPP__
#define INCLUDED__apiADatabase_MySQL_HPP__

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef ADATABASE_MYSQL_EXPORTS
#      pragma message("ADatabase_MySQL: EXPORT")
#      define ADATABASE_MYSQL_API __declspec(dllexport)
#      pragma comment(lib, "libmysql")
#    else
#      pragma message("ADatabase_MySQL: IMPORT")
#      define ADATABASE_MYSQL_API __declspec(dllimport)
#      pragma comment(lib, "ADatabase_MySQL")
#    endif
#  else
#    pragma message("ADatabase_MySQL: NONE")
#    define ADATABASE_MYSQL_API
#  endif
#else
#define ADATABASE_MYSQL_API
#endif

#include "apiABase.hpp"

#include "mysql.h"

#endif //INCLUDED__apiADatabase_MySQL_HPP__
