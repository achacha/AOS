#include "pchAOS_Test.hpp"
#include "apiAOS_Test.hpp"
#include "AObjectContainer.hpp"

#include "AOSModule_Test.hpp"
#include "AOSModule_TestFail.hpp"
#include "AOSModule_SessionTest.hpp"

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

extern "C" AOS_TEST_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  ALog& alog
)
{
  alog.add(ASWNL("AOS_Test: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_Test(alog));
  moduleExecutor.registerModule(new AOSModule_TestFail(alog));
  moduleExecutor.registerModule(new AOSModule_SessionTest(alog));
  
  return 0;
}

extern "C" AOS_TEST_API int aos_init(AOSServices& services)
{
  services.useLog().add(ASWNL("AOS_Test: aos_init"), ALog::INFO);

  return 0;
}
