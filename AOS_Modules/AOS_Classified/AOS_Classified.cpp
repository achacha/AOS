#include "pchAOS_Classified.hpp"
#include "apiAOS_Classified.hpp"

#include "AOSModule_classified_input.hpp"
#include "AOSModule_classified_submit.hpp"
#include "AOSModule_classified_list.hpp"

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

extern "C" AOS_CLASSIFIED_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_Classified: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_classified_input(services));
  moduleExecutor.registerModule(new AOSModule_classified_submit(services));
  moduleExecutor.registerModule(new AOSModule_classified_list(services));
  
  return 0;
}
