/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_Example.hpp"
#include "apiAOS_Example.hpp"

#include "AOSModule_RSS20.hpp"
#include "AOSOutput_generate_image.hpp"

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

extern "C" AOS_EXAMPLE_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_Example: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_rss20(services));
  
  //Register output generators
  outputExecutor.registerOutputGenerator(new AOSOutput_generate_image(services));

  return 0;
}
