/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAGdLib.hpp"

BOOL APIENTRY DllMain( 
  HANDLE, 
  DWORD ul_reason_for_call, 
  LPVOID
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

