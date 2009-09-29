/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiABase_HPP__
#define INCLUDED__apiABase_HPP__

// Enable to allow windows memory tracking
//#define WINDOWS_CRTDBG_ENABLED 1

// DEBUG_TRACK_ABASE_MEMORY - Enable to track ABase* allocations
// DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT - When DEBUG_TRACK_ABASE_MEMORY is defined, this will trace allocations to std::cout
//#define DEBUG_TRACK_ABASE_MEMORY 1
//#define DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT 1

//a_OS Defines and compiler specifics
#include "osDefines.hpp"

//a_Define DLL export linkage for MSDEV
//a_If _ISOLATION is defined then CPPs can be included into a console app
//a_Static libraries (_LIB) do not need explicit export
//a_export linkage for current dynamic object
//a_import linkage for all others
//a_no linkage for isolation
#if defined(_MSC_VER) && defined(__WINDOWS__)
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

//a_Used for debugging, this is the maximum sizes used
//a_Only used when __DEBUG_DUMP__ is set
#ifdef __DEBUG_DUMP__
// Max size
#define DEBUG_MAXSIZE_AString 5000000
#define DEBUG_MAXSIZE_ARope 30000000
#define DEBUG_MAXSIZE_ARopeDeque 30000000

// Max items
#define DEBUG_MAXSIZE_ABasePtrQueue 100000
#define DEBUG_MAXSIZE_ABasePtrContainer 100000

// Max number of HTTP tokens
#define DEBUG_MAXSIZE_AHTTPHeader 1000

// Max size of container
#define DEBUG_MAXSIZE_AXmlElement 3000

// Attributes items
#define DEBUG_MAXSIZE_AXmlAttributes 300

// Max items per cache
#define DEBUG_MAXSIZE_ACache_FileSystem 100000

// Max size (size * 8 = bits)
#define DEBUG_MAXSIZE_ABitArray 10000

// Max rows
#define DEBUG_MAXSIZE_AResultSet 500000

// Enable windows specific stacktrace in debug mode
#ifdef __WINDOWS__

#ifndef DISABLE_EXCEPTION_STACKWALK
#define ENABLE_EXCEPTION_STACKWALK 1
#pragma message("Enabling Stack Walker exceptions")
#else
#undef ENABLE_EXCEPTION_STACKWALK
#pragma message("Disabling Stack Walker exceptions")
#endif

#endif
#endif

//a_Basic ABase classes
#include "AConstant.hpp"

//a_Version
#define ABase_INFO "ABase 1.2.7 " ## ABASE_OS_INFO

#endif //INCLUDED__apiABase_HPP__
