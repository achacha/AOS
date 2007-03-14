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

//a_Include api headers for other libararies which then import the appropriate LIBs
#include <apiABase.hpp>

typedef int (PROC_AOS_Register)(AOSInputExecutor&, AOSModuleExecutor&, AOSOutputExecutor&, ALog&);
typedef int (PROC_AOS_Init)(AOSServices&);

#ifdef WIN32
#define AOS_SERVER_NAME ASW("AsynchObjectServer/1.0.2.0",26)
#define AOS_ADMIN_SERVER_NAME ASW("AOSAdminServer/1.2.0.1",22)
#endif

#endif //__apiAOS_Base_HPP__
