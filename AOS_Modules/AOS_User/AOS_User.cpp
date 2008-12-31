/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_User.hpp"
#include "apiAOS_User.hpp"

#include "AOSModule_User_LoginController.hpp"
#include "AOSModule_User_Authenticate.hpp"
#include "AOSModule_User_Logout.hpp"
#include "AOSModule_Captcha_validate.hpp"
#include "AOSOutput_Captcha_generate.hpp"

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

extern "C" AOS_USER_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_User: aos_register"), ALog::EVENT_INFO);

  //Register modules
  moduleExecutor.registerModule(new AOSModule_User_LoginController(services));
  moduleExecutor.registerModule(new AOSModule_User_Authenticate(services));
  moduleExecutor.registerModule(new AOSModule_User_Logout(services));
  moduleExecutor.registerModule(new AOSModule_Captcha_validate(services));
  
  //Register output generators
  outputExecutor.registerOutputGenerator(new AOSOutput_Captcha_generate(services));
  
  return 0;
}
