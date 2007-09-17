#ifndef INCLUDED__apiABase_HPP__
#define INCLUDED__apiABase_HPP__

//a_OS Defines and compiler specifics
#include "osDefines.hpp"

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
//a_Static libraries (_LIB) do not need explicit export
//a_export linkage for current dynamic object
//a_import linkage for all others
//a_no linkage for isolation
#if defined(_MSC_VER)
#  if !defined(__ISOLATION__)
#    ifdef ABASE_EXPORTS
#      pragma message("ABase: EXPORT")
#      define ABASE_API __declspec(dllexport)
#      pragma comment(lib, "kernel32")
#      pragma comment(lib, "user32")
#      pragma comment(lib, "advapi32")
#      pragma comment(lib, "wsock32")
#      pragma comment(lib, "ws2_32")
#      pragma comment(lib, "iphlpapi")
#    else
#      pragma message("ABase: IMPORT")
#      define ABASE_API __declspec(dllimport)
#      pragma comment(lib, "ABase")
#    endif
#  else
#    pragma message("ABase: NONE")
#    define ABASE_API
#  endif
#else
#define ABASE_API
#endif

//a_External dependancies
#include "extABase.hpp"

//a_Basic types
#include "uTypes.hpp"

//a_Macros
#include "macroDelete.hpp"

//a_Constants
#include "AConstant.hpp"

#endif //INCLUDED__apiABase_HPP__
