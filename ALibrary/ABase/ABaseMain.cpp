/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#pragma hdrstop

#ifdef __WINDOWS__
BOOL APIENTRY DllMain( HANDLE,
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
#endif //__WINDOWS__
