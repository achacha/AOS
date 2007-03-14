#include "apiABase.hpp"
#include "apiASystem.hpp"
#include "AString.hpp"
#include "ASystemException.hpp"
#include "ASocketLibrary.hpp"

ASocketLibrary g_SocketLibrary;
PROCESS_INFORMATION g_ProcessInformation;
STARTUPINFO g_StartupInfo;

int main()
{
  memset(&g_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  memset(&g_StartupInfo, 0, sizeof(STARTUPINFO)); 

  g_StartupInfo.cb = sizeof(STARTUPINFO);  
  g_StartupInfo.lpTitle = "AObjectServer";  

  AString strPath = "./AObjectServer.exe";

  BOOL success = ::CreateProcess(
    "./AObjectServer.exe",   //LPCTSTR lpApplicationName,
    NULL,                    //LPTSTR lpCommandLine,
    NULL,                    //LPSECURITY_ATTRIBUTES lpProcessAttributes,
    NULL,                    //LPSECURITY_ATTRIBUTES lpThreadAttributes,
    FALSE,                   //BOOL bInheritHandles,
    DETACHED_PROCESS,        //DWORD dwCreationFlags,
    NULL,                    //LPVOID lpEnvironment,
    NULL,                    //LPCTSTR lpCurrentDirectory,
    &g_StartupInfo,           //LPSTARTUPINFO lpStartupInfo,
    &g_ProcessInformation     //LPPROCESS_INFORMATION lpProcessInformation
  );

  if (!success)
  {
    ASystemException sysEx(::GetLastError());
    std::cerr << sysEx.what() << std::endl;
  }

  AString strDebug("PROCINFO:");
  strDebug.append(success ? "TRUE" : "FALSE");
  strDebug.append(": process_id=");
  strDebug.append(AString::fromU4(g_ProcessInformation.dwProcessId));
  strDebug.append("  thread_id=");
  strDebug.append(AString::fromU4(g_ProcessInformation.dwThreadId));
  strDebug.append("  hProcess=");
  strDebug.append(AString::fromPointer((void *)g_ProcessInformation.hProcess));
  strDebug.append("  hThread=");
  strDebug.append(AString::fromPointer((void *)g_ProcessInformation.hThread));
  std::cout << strDebug << std::endl;
  
  return 0;
}
