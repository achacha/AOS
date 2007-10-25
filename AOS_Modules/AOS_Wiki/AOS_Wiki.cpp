#include "pchAOS_Wiki.hpp"
#include "apiAOS_Wiki.hpp"

#include "AOSModule_WikiRemapPath.hpp"
#include "AOSModule_WikiView.hpp"

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

extern "C" AOS_WIKI_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_Wiki: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_WikiRemapPath(services));
  moduleExecutor.registerModule(new AOSModule_WikiView(services));
  
  return 0;
}
