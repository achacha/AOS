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
#include "AOSModule_ExecuteQuery.hpp"
#include "AOSModule_AlterContext.hpp"
#include "AOSModule_InsertIntoModel.hpp"
#include "AOSModule_Template.hpp"
#include "AOSModule_LuaScript.hpp"
#include "AOSModule_FileList.hpp"
#include "AOSModule_SaveToFile.hpp"

//a_Output generators
#include "AOSOutput_NOP.hpp"
#include "AOSOutput_JSON.hpp"
#include "AOSOutput_MsXslt.hpp"
#include "AOSOutput_Template.hpp"
#include "AOSOutput_File.hpp"
#include "AOSOutput_Redirect.hpp"

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

extern "C" AOS_BASEMODULES_API int aos_register(
  AOSInputExecutor& inputExecutor, 
  AOSModuleExecutor& moduleExecutor, 
  AOSOutputExecutor& outputExecutor, 
  AOSServices& services
)
{
  services.useLog().add(ASWNL("AOS_BaseModules: aos_register"), ALog::INFO);

  //Register input processors
  inputExecutor.registerInputProcessor(new AOSInput_HtmlForm(services));
  inputExecutor.registerInputProcessor(new AOSInput_NOP(services));
  inputExecutor.registerInputProcessor(new AOSInput_HtmlFormMultiPart(services));

  //Register modules
  moduleExecutor.registerModule(new AOSModule_NOP(services));
  moduleExecutor.registerModule(new AOSModule_ExecuteQuery(services));
  moduleExecutor.registerModule(new AOSModule_AlterContext(services));
  moduleExecutor.registerModule(new AOSModule_InsertIntoModel(services));
  moduleExecutor.registerModule(new AOSModule_Template(services));
  moduleExecutor.registerModule(new AOSModule_LuaScript(services));
  moduleExecutor.registerModule(new AOSModule_FileList(services));
  moduleExecutor.registerModule(new AOSModule_SaveToFile(services));
  
  //Register output generators
  outputExecutor.registerOutputGenerator(new AOSOutput_NOP(services));
  outputExecutor.registerOutputGenerator(new AOSOutput_JSON(services));
  outputExecutor.registerOutputGenerator(new AOSOutput_Template(services));
  outputExecutor.registerOutputGenerator(new AOSOutput_File(services));
  outputExecutor.registerOutputGenerator(new AOSOutput_Redirect(services));

  //a_XSLT is OS specific, on Windows we use MSXML6 which so far is the fastest one around
#ifdef __WINDOWS__
  outputExecutor.registerOutputGenerator(new AOSOutput_MsXslt(services));
#endif

  return 0;
}
