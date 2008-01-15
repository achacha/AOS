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
class AObjectContainer;
class AOSServices;
class ALog;

#define ENABLE_AFILE_TRACER_DEBUG                      // This will enable it, comment it out to disable
#define USE_WINDOWS_DEBUG_OUTPUT                       //OPTIONAL: Instead of a file, redirect output to Windows debug output

//a_Include api headers for other libararies which then import the appropriate LIBs
#include <apiABase.hpp>

class ASync_CriticalSection;
AOS_BASE_API extern ASync_CriticalSection g_DebugTraceSync; 
#include <debugFileTracer.hpp>
#define AOS_DEBUGTRACE(msg, ptr) do { \
  ALock lock(g_DebugTraceSync); \
  AFILE_TRACER_DEBUG_MESSAGE(msg, ptr); \
  std::cout << msg << std::endl; \
} while(0)

typedef int (PROC_AOS_Register)(AOSInputExecutor&, AOSModuleExecutor&, AOSOutputExecutor&, AOSServices&);

#ifdef WIN32
#define AOS_SERVER_NAME "Conveyer/1.3.0.0 [en]"
#define AOS_ADMIN_SERVER_NAME "Conveyer Admin Server/1.2.0.0 [en]"

#endif

#endif //__apiAOS_Base_HPP__
