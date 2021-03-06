#include "pchAOS_SAMPLEPROJECT.hpp"
#include "apiAOS_SAMPLEPROJECT.hpp"

#include "AOSModule_SAMPLEMODULE.hpp"

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

extern "C" AOS_SAMPLEPROJECTUPPERCASE_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_SAMPLEPROJECT: aos_register"), ALog::EVENT_INFO);

  //Register input processors
  //inputExecutor.registerInputProcessor(...);

  //Register modules
  //moduleExecutor.registerModule(...);
  moduleExecutor.registerModule(new AOSModule_SAMPLEMODULE(services));
  
  //Register output generators
  //outputExecutor.registerOutputGenerator(...);

  return 0;
}
