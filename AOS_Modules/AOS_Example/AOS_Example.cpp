#include "pchAOS_Example.hpp"
#include "apiAOS_Example.hpp"

#include "AOSModule_classified_input.hpp"
#include "AOSModule_classified_submit.hpp"
#include "AOSModule_classified_list.hpp"
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
  ALog& alog
)
{
  alog.add(ASWNL("AOS_Example: aos_register"), ALog::INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_classified_input(alog));
  moduleExecutor.registerModule(new AOSModule_classified_submit(alog));
  moduleExecutor.registerModule(new AOSModule_classified_list(alog));
  moduleExecutor.registerModule(new AOSModule_rss20(alog));
  
  //Register output generators
  outputExecutor.registerOutputGenerator(new AOSOutput_generate_image(alog));

  return 0;
}

extern "C" AOS_EXAMPLE_API int aos_init(AOSServices& services)
{
  services.useLog().add(ASWNL("AOS_Example: aos_init"), ALog::INFO);

  return 0;
}
