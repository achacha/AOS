/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef __apiAOS_Base_HPP__
#define __apiAOS_Base_HPP__

#ifdef AOS_BASE_EXPORTS
#  pragma message("AOS_Base: EXPORT")
#  define AOS_BASE_API __declspec(dllexport)
#else
#  pragma message("AOS_Base: IMPORT")
#  define AOS_BASE_API __declspec(dllimport)
#  pragma comment(lib, "AOS_Base")
#endif

//a_Signature for the proc that does the initialization
class AOSInputExecutor;
class AOSModuleExecutor;
class AOSOutputExecutor;
class AOSServices;
class ALog;

#define ENABLE_AFILE_TRACER_DEBUG                      // This will enable it, comment it out to disable
#define DEBUGFILETRACER_USE_WINDOWS_DEBUG_OUTPUT                       //OPTIONAL: Instead of a file, redirect output to Windows debug output

//a_Include api headers for other libararies which then import the appropriate LIBs
#include <apiABase.hpp>
#include <apiAPlatform.hpp>

class ASync_CriticalSection;
AOS_BASE_API extern ASync_CriticalSection g_DebugTraceSync; 
#include <debugFileTracer.hpp>
#define AOS_DEBUGTRACE(msg, ptr) { \
  ALock lock(g_DebugTraceSync); \
  AFILE_TRACER_DEBUG_MESSAGE(msg, ptr); \
  std::cout << msg << std::endl; \
}

typedef int (PROC_AOS_Register)(AOSInputExecutor&, AOSModuleExecutor&, AOSOutputExecutor&, AOSServices&);

#define AOS_SERVER_NAME "AOS Rhino/1.8.0.0"
#define AOS_SERVER_INFO "Rhino/1.8.0.0 " ## ABASE_OS_INFO
#define AOS_ADMIN_SERVER_NAME "AOS Rhino Admin Server/1.4.0.0"
#define AOS_Base_INFO AOS_SERVER_INFO ## " " ## __BUILDTYPE__ ## " Build(" ## __TIMESTAMP__ ## ")"

#endif //__apiAOS_Base_HPP__
