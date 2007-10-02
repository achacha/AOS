// AOS_BaseModules.cpp : Defines the entry point for the DLL application.
//

#include "pchAOS_BaseModules.hpp"
#include "apiAOS_BaseModules.hpp"

//a_Input processors
#include "AOSInput_NOP.hpp"
#include "AOSInput_HtmlForm.hpp"
#include "AOSInput_HtmlFormMultiPart.hpp"

//a_Modules
#include "AOSModule_NOP.hpp"
#include "AOSModule_PublishInput.hpp"
#include "AOSModule_ExecuteQuery.hpp"
#include "AOSModule_AlterContext.hpp"
#include "AOSModule_InsertIntoModel.hpp"

//a_Output generators
#include "AOSOutput_NOP.hpp"
#include "AOSOutput_JSON.hpp"
#include "AOSOutput_MsXslt.hpp"
#include "AOSOutput_XalanXslt.hpp"
#include "AOSOutput_Template.hpp"
#include "AOSOutput_File.hpp"

//a_Main DLL entry point
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

extern "C" AOS_BASEMODULES_API int aos_init(AOSServices& services)
{
  services.useLog().add(ASWNL("AOS_BaseModules: aos_init"), ALog::INFO);

  return 0;
}

extern "C" AOS_BASEMODULES_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  ALog& alog
)
{
  alog.add(ASWNL("AOS_BaseModules: aos_register"), ALog::INFO);

  //Register input processors
  inputExecutor.registerInputProcessor(new AOSInput_HtmlForm(alog));
  inputExecutor.registerInputProcessor(new AOSInput_NOP(alog));
  inputExecutor.registerInputProcessor(new AOSInput_HtmlFormMultiPart(alog));

  //Register modules
  moduleExecutor.registerModule(new AOSModule_NOP(alog));
  moduleExecutor.registerModule(new AOSModule_PublishInput(alog));
  moduleExecutor.registerModule(new AOSModule_ExecuteQuery(alog));
  moduleExecutor.registerModule(new AOSModule_AlterContext(alog));
  moduleExecutor.registerModule(new AOSModule_InsertIntoModel(alog));
  
  //Register output generators
  outputExecutor.registerOutputGenerator(new AOSOutput_NOP(alog));
  outputExecutor.registerOutputGenerator(new AOSOutput_JSON(alog));
#ifdef AOS__USE_MSXML4__
  outputExecutor.registerOutputGenerator(new AOSOutput_MsXslt(alog));
#endif
  outputExecutor.registerOutputGenerator(new AOSOutput_XalanXslt(alog));
  outputExecutor.registerOutputGenerator(new AOSOutput_Template(alog));
  outputExecutor.registerOutputGenerator(new AOSOutput_File(alog));

  return 0;
}
