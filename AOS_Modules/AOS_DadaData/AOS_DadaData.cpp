/*
Written by Alex Chachanashvili

$Id$
*/

#include "pchAOS_DadaData.hpp"
#include "apiAOS_DadaData.hpp"

#include "ATemplateNodeHandler_DADA.hpp"

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

extern "C" AOS_DADADATA_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_DadaData: aos_register"), ALog::EVENT_INFO);

  //Register the DADA template handler
  services.addTemplateHandler(new ATemplateNodeHandler_DADA(services));

  return 0;
}
