/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Test.hpp"
#include "apiAOS_Test.hpp"

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
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_Test: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_Test(services));
  moduleExecutor.registerModule(new AOSModule_TestFail(services));
  moduleExecutor.registerModule(new AOSModule_SessionTest(services));
  
  return 0;
}
