/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__osDefines_HPP__
#define INCLUDED__osDefines_HPP__

/*
Visual C++ warning disable
*/
#if _MSC_VER > 1000
  #pragma warning(disable:4706)  // assignment within conditional expression
  #pragma warning(disable:4786)  // identifier was truncated to '255' characters in the debug information
  #pragma warning(disable:4251)  // class 'xxx' needs to have dll-interface to be used by clients of class 'xxx'
  #pragma warning(disable:6011)  // Dereferencing NULL pointer (STL deque warning)
  #pragma warning(disable:4245)  // 'argument' : conversion from '' to 'size_t', signed/unsigned mismatch (due to use of enum in AConstant for npos/etc)
  #pragma warning(disable:4512)  // assignment operator could not be generated (some classes have reference members to prevent copy)
  #pragma warning(disable:4100)  // unreferenced formal parameter
#endif

/** What do the OS/Compiler defines mean
NEEDS_ANSI_HELPERS - OS/C needs: strrev strnicmp xtoa itoa ltoa ultoa
HAS_MUTABLE - OS/C has 'mutable' keyword
HAS_BOOL - OS/C has 'bool' keyword (will use unsigned long otherwise with 1 and 0 for true and false)
LITTLE_ENDIAN - processor specific, intel is big endian, motorola is little endian, etc (default big endian)
**/
#if defined(_WIN64) || defined(WIN64)
  //Win64 environment
  #pragma message("ABase (64bit): Windows OS detected")
  #define __WINDOWS__
  #ifndef WIN64
  #  define WIN64
  #endif
  #ifndef WIN32
  #  define WIN32
  #endif

  #ifndef WINVER			          // Allow use of features specific to Windows XP or later.
  #define WINVER 0x0501		      // Change this to the appropriate value to target other versions of Windows.
  #endif

  #ifndef _WIN32_WINNT	     	  // Allow use of features specific to Windows XP or later.
  #define _WIN32_WINNT 0x0501	  // Change this to the appropriate value to target other versions of Windows.
  #endif

#ifndef _WIN32_WINDOWS		      // Allow use of features specific to Windows 98 or later.
  #define _WIN32_WINDOWS 0x0501 // Target Windows Me or better (Windows XP uses _WIN32_WINNT define, so this one is really pretty useless nowadays)
  #endif

//a_OS/C flags
  #define HAS_MUTABLE
  #define HAS_BOOL

  #define __CDECL __cdecl

  //a_Predefine flags needed with Microsoft headers
  #undef FD_SETSIZE
  #define FD_SETSIZE 1024             //a_Allow 1024 simultaneous clients

  //a_Include Microsoft Windows headers
  //#define WIN64_LEAN_AND_MEAN		      // Exclude rarely-used stuff from Windows headers
  #ifndef __AFX_H__
    #pragma message("ABase (64 bit): Including windows.h")
    #include <windows.h>   //a_Include windows.h if this is not an MFC app
  #endif

  #define ABASE_OS_INFO "64-bit Windows"
#elif defined(_WIN32) || defined(WIN32)
  //Win32 environment
  #pragma message("ABase (32bit): Windows OS detected")
  #define __WINDOWS__
  #ifndef WIN32
  #  define WIN32
  #endif

  //a_OS/C flags
  #define HAS_MUTABLE
  #define HAS_BOOL

  #ifndef WINVER                // Allow use of features specific to Windows XP or later.
  #define WINVER 0x0501         // Change this to the appropriate value to target other versions of Windows.
  #endif

  #ifndef _WIN32_WINNT          // Allow use of features specific to Windows XP or later.
  #define _WIN32_WINNT 0x0501   // Change this to the appropriate value to target other versions of Windows.
  #endif

  #ifndef _WIN32_WINDOWS		    // Allow use of features specific to Windows 98 or later.
  #define _WIN32_WINDOWS 0x0501 // Target Windows Me or better (Windows XP uses _WIN32_WINNT define, so this one is really pretty useless nowadays)
  #endif

  #define __CDECL __cdecl

  //a_Predefine flags needed with Microsoft headers
  #undef FD_SETSIZE
  #define FD_SETSIZE 1024             //a_Allow 1024 simultaneous clients

  #define WIN32_LEAN_AND_MEAN		      // Exclude rarely-used stuff from Windows headers
  //a_Include Microsoft Windows headers
  #ifndef __AFX_H__
    #pragma message("ABase (32bit): Including windows.h")
    #include <windows.h>   //a_Include windows.h if this is not an MFC app
    #pragma message("ASystem (32bit): Including winsock2.h")
    #include <winsock2.h>
  #endif

#define ABASE_OS_INFO "32-bit Windows"
#elif defined(LINUX32)
  #pragma message("ABase (32bit): Linux OS detected")
  #define __LINUX__

  //a_OS/C flags
  #define HAS_MUTABLE
  #define HAS_BOOL

  #undef FD_SETSIZE
  #define FD_SETSIZE 1024             //a_Allow 1024 simultaneous clients

  #define ABASE_OS_INFO "32-bit Linux"
#elif defined(LINUX64)
  #error TODO Linux 64-bit
  #define ABASE_OS_INFO "64-bit Linux"
#else
  #error No such OS configuration
#endif

/*!
DEBUG/RELEASE compiler flag

_DEBUG - debug mode
NDEBUG - non-debug (release) mode

*/
#if !defined(__DEBUG_DUMP__)
  #define __DEBUG_DUMP__
  #pragma message("__osDefines.hpp: ADebugDumpable::debugDump enabled")
#endif
#if defined(_DEBUG)
  #pragma message("osDefines.hpp: Debug Mode")
  #define __BUILDTYPE__ "Debug"
#elif defined(NDEBUG)
  #pragma message("osDefines.hpp: Release Mode")
  #define __BUILDTYPE__ "Optimized"
#else
  #error Must define _DEBUG or NDEBUG to define debug or release mode
#endif

//a_Microsoft CRT debugging
//
// Generates memory leak report
// DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(false) - creates memory snapshot at startup and output is to debug Output windows
// DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(true)  - creates memory snapshot at startup and output is to STDOUT
// DEBUG_MEMORY_LEAK_ANALYSIS_END()   - creates memory diff and reports leaks
//
//Usage:
//
// int main()
// {
//   DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(false);
//   ...do stuff here...
//   DEBUG_MEMORY_LEAK_ANALYSIS_END();
//   return 0;
// }



//
// See apiABase.hpp where it is defined
//

//EXPERIMENTAL: Enbale trace of allocations of ABase* objects (doesn't always work)
// Takes a very long time too, so best keep this commented out unless really needed for debugging
#  ifdef DEBUG_TRACK_ABASE_MEMORY
#  pragma message("Tracing dynamic allocations of ABase* to std::cout")
#  endif

// Enable windows specific stacktrace in debug mode
#ifdef __WINDOWS__

#if defined(DISABLE_EXCEPTION_STACKWALK) && defined(_DEBUG)
//TODO Current stack walker is not thread safe
//#define ENABLE_EXCEPTION_STACKWALK 1
#undef ENABLE_EXCEPTION_STACKWALK
#pragma message("Enabling Stack Walker exceptions")
#else
#undef ENABLE_EXCEPTION_STACKWALK
#pragma message("Disabling Stack Walker exceptions")
#endif


#  if defined(_DEBUG) && defined(WINDOWS_CRTDBG_ENABLED)
#  define _CRTDBG_MAP_ALLOC 1
#  include "crtdbg.h"
#  pragma message("Enabling Microsoft CRT debugging and memory tracking")

#  define DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(output2stdout) \
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF| _CRTDBG_ALLOC_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF);\
  _CrtMemState _memstate_start;\
  _CrtMemState _memstate_end;\
  _CrtMemState _memstate_diff;\
  _CrtMemCheckpoint(&_memstate_start);\
  if (output2stdout) {\
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );\
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );\
  }

// Can be used to set a different start point after above macro is used
#define DEBUG_MEMORY_SET_START_CHECKPOINT() \
  _CrtMemCheckpoint(&_memstate_start);


#  define DEBUG_MEMORY_LEAK_ANALYSIS_END() \
  _CrtCheckMemory();\
  _CrtMemCheckpoint(&_memstate_end);\
  if (_CrtMemDifference(&_memstate_diff, &_memstate_start, &_memstate_end))\
  {\
    _CrtMemDumpStatistics(&_memstate_diff);\
    /*_CrtMemDumpAllObjectsSince(&_memstate_start);*/\
    _CrtDumpMemoryLeaks();\
  }
#  else
#  define DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(a) (void *)0
#  define DEBUG_MEMORY_SET_START_CHECKPOINT() (void *)0
#  define DEBUG_MEMORY_LEAK_ANALYSIS_END()    (void *)0
#  endif // _DEBUG
#endif // __WINDOWS___

#endif  //a_ INCLUDED__osDefines_HPP__
