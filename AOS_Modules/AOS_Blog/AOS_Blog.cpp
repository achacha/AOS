/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Blog.hpp"
#include "apiAOS_Blog.hpp"

#include "AOSModule_BlogView.hpp"

BOOL APIENTRY DllMain(
  HANDLE hModule, 
  DWORD  ul_reason_for_call, 
  LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

extern "C" AOS_BLOG_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_Blog: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_BlogView(services));
  
  return 0;
}
