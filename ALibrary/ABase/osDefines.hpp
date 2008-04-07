#ifndef INCLUDED__osDefines_HPP__
#define INCLUDED__osDefines_HPP__

/** What do the OS/Compiler defines mean
NEEDS_ANSI_HELPERS - OS/C needs: strrev strnicmp xtoa itoa ltoa ultoa
HAS_MUTABLE - OS/C has 'mutable' keyword
HAS_BOOL - OS/C has 'bool' keyword (will use unsigned long otherwise with 1 and 0 for true and false)
LITTLE_ENDIAN - processor specific, intel is big endian, motorola is little endian, etc (default big endian)
**/
#if defined(_WIN64)
  //Win64 environment
  #define __WINDOWS__
  #pragma message("ABase (64bit): Windows OS detected")

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
    //#pragma message("ASystem (64bit): Including winsock2.h")
    //#include <winsock2.h>
  #endif
#elif defined(_WIN32)
  //Win32 environment
  #define __WINDOWS__
  #pragma message("ABase (32bit): Windows OS detected")

  //a_OS/C flags
  #define HAS_MUTABLE
  #define HAS_BOOL
  
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Target Windows Me or better (Windows XP uses _WIN32_WINNT define, so this one is really pretty useless nowadays)
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
  
#if _MSC_VER > 1000
  #pragma once
#endif
  
  #pragma warning (disable:4706)  // assignment within conditional expression
  #pragma warning (disable:4786)  // identifier was truncated to '255' characters in the debug information
  #pragma warning (disable:4251)  // class 'xxx' needs to have dll-interface to be used by clients of class 'xxx'
//  #pragma warning (disable:6011)  // Dereferencing NULL pointer (STL deque warning)
  #pragma warning (disable:4245)  // 'argument' : conversion from '' to 'size_t', signed/unsigned mismatch (due to use of enum in AConstant for npos/etc)
  #pragma warning(disable:4512)   // assignment operator could not be generated (some classes have reference members to prevent copy)
  #pragma warning(disable:4100)   // unreferenced formal parameter

#elif defined (__unix)
  //a_For all UNIX
  //a_Defines to be used inside the event log class (invisible to the user, since there is an enum to shield us)
  #define EVENTLOG_INFORMATION_TYPE 1
  #define EVENTLOG_WARNING_TYPE     2
  #define EVENTLOG_ERROR_TYPE       3

  //a_Unix platform
  #define __UNIX__

  //a_Some flavor of unix 
  #if defined(__SUNPRO_CC) && defined(__sun)
    
    //a_Solaris
    #define __SUNOS__
    #pragma message("SunOS detected")

    //a_OS/C flags
    #define NEEDS_ANSI_HELPERS

    #define __CDECL

    //a_Solaris 2.51 SunC 4.2 is not ANSI C++ 2.0 compliant
		#ifndef bool
		typedef unsigned long bool;
		#endif
		#ifndef true
		#define true 1
		#endif
		#ifndef false
		#define false 0
		#endif
		#ifndef explicit
		#define explicit
		#endif
		#ifndef mutable
		#define mutable
		#endif

#elif defined(__linux)
  
    //a_Linux
    #define __LINUX__
    #pragma message("Linux OS detected")

    //a_OS/C flags
    #define NEEDS_ANSI_HELPERS

    #define __CDECL

  #elif defined (__CYGWIN32__)
  #else
    #error OS configuration not supported

  #endif
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
#elif defined(NDEBUG)
  #pragma message("osDefines.hpp: Release Mode")
#else
  #error Must define _DEBUG or NDEBUG to define debug or release mode
#endif

#endif  //a_ INCLUDED__osDefines_HPP__
