#include "pchAOS_Forum.hpp"
#include "apiAOS_Forum.hpp"

#include "AOSModule_forum_list.hpp"

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

extern "C" AOS_FORUM_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  ALog& alog
)
{
  alog.add(ASWNL("AOS_Forum: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_forum_list(alog));
  
  return 0;
}

extern "C" AOS_FORUM_API int aos_init(AOSServices& services)
{
  services.useLog().add(ASWNL("AOS_Forum: aos_init"), ALog::INFO);

  return 0;
}
