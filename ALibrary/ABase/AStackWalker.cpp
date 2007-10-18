/**********************************************************************
 * 
 * StackWalker.cpp
 *
 *
 * History:
 *  2005-07-27   v1    - First public release on http://www.codeproject.com/
 *                       http://www.codeproject.com/threads/StackWalker.asp
 *  2005-07-28   v2    - Changed the params of the constructor and ProcessCallstack
 *                       (to simplify the usage)
 *  2005-08-01   v3    - Changed to use 'CONTEXT_FULL' instead of CONTEXT_ALL 
 *                       (should also be enough)
 *                     - Changed to compile correctly with the PSDK of VC7.0
 *                       (GetFileVersionInfoSizeA and GetFileVersionInfoA is wrongly defined:
 *                        it uses LPSTR instead of LPCSTR as first paremeter)
 *                     - Added declarations to support VC5/6 without using 'dbghelp.h'
 *                     - Added a 'pUserData' member to the ProcessCallstack function and the 
 *                       PReadProcessMemoryRoutine declaration (to pass some user-defined data, 
 *                       which can be used in the readMemoryFunction-callback)
 *  2005-08-02   v4    - OnSymInit now also outputs the OS-Version by default
 *                     - Added example for doing an exception-callstack-walking in main.cpp
 *                       (thanks to owillebo: http://www.codeproject.com/script/profile/whos_who.asp?id=536268)
 *
 **********************************************************************/
#include "pchABase.hpp"
#include <tchar.h>
#pragma comment(lib, "version.lib")  // for "VerQueryValue"

#include "AStackWalker.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"
#include "ALock.hpp"

ASync_CriticalSection AStackWalker::sm_StackWalkLock;

// The "ugly" assembler-implementation is needed for systems before XP
// If you have a new PSDK and you only compile for XP and later, then you can use 
// the "RtlCaptureContext"
// Currently there is no define which determines the PSDK-Version... 
// So we just use the compiler-version (and assumes that the PSDK is 
// the one which was installed by the VS-IDE)
//#if defined(_M_IX86) && (_WIN32_WINNT <= 0x0500) //&& (_MSC_VER < 1400)
#if !defined(RtlCaptureContext) && defined(_M_IX86)
  #ifdef CURRENT_THREAD_VIA_EXCEPTION
  // TODO: The following is not a "good" implementation, 
  // because the callstack is only valid in the "__except" block...
  #define GET_CURRENT_CONTEXT(context, contextFlags) \
    do { \
      memset(&context, 0, sizeof(CONTEXT)); \
      EXCEPTION_POINTERS *pExp = NULL; \
      __try { \
        throw 0; \
      } __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
      if (pExp != NULL) \
        memcpy(&context, pExp->ContextRecord, sizeof(CONTEXT)); \
        context.ContextFlags = contextFlags; \
    } while(0);
  #else
  // The following should be enough for walking the callstack...
  #define GET_CURRENT_CONTEXT(context, contextFlags) \
    do { \
      memset(&context, 0, sizeof(CONTEXT)); \
      context.ContextFlags = contextFlags; \
      __asm    call x \
      __asm x: pop eax \
      __asm    mov context.Eip, eax \
      __asm    mov context.Ebp, ebp \
      __asm    mov context.Esp, esp \
    } while(0);
  #endif
#else
  // The following is defined for x86 (XP and higher), x64 and IA64:
  #define GET_CURRENT_CONTEXT(context, contextFlags) \
    do { \
      memset(&context, 0, sizeof(CONTEXT)); \
      context.ContextFlags = contextFlags; \
      RtlCaptureContext(&context); \
  } while(0);
#endif

// If VC7 and later, then use the shipped 'dbghelp.h'-file
#if _MSC_VER >= 1300
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#else
// inline the important dbghelp.h-declarationstackframe...
typedef enum {
    SymNone = 0,
    SymCoff,
    SymCv,
    SymPdb,
    SymExport,
    SymDeferred,
    SymSym,
    SymDia,
    SymVirtual,
    NumSymTypes
} SYM_TYPE;
typedef struct _IMAGEHLP_LINE64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_LINE64)
    PVOID                       Key;                    // internal
    DWORD                       LineNumber;             // line number in file
    PCHAR                       FileName;               // full filename
    DWORD64                     Address;                // first instruction of line
} IMAGEHLP_LINE64, *PIMAGEHLP_LINE64;
typedef struct _IMAGEHLP_MODULE64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64                     BaseOfImage;            // base load address of module
    DWORD                       ImageSize;              // virtual size of the loaded module
    DWORD                       TimeDateStamp;          // date/time stamp from pe header
    DWORD                       CheckSum;               // checksum from the pe header
    DWORD                       NumSyms;                // number of symbols in the symbol table
    SYM_TYPE                    SymType;                // type of symbols loaded
    CHAR                        ModuleName[32];         // module name
    CHAR                        ImageName[256];         // image name
    CHAR                        LoadedImageName[256];   // symbol file name
} IMAGEHLP_MODULE64, *PIMAGEHLP_MODULE64;
typedef struct _IMAGEHLP_SYMBOL64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_SYMBOL64)
    DWORD64                     Address;                // virtual address including dll base address
    DWORD                       Size;                   // estimated size of symbol, can be zero
    DWORD                       Flags;                  // info about the symbols, see the SYMF defines
    DWORD                       MaxNameLength;          // maximum size of symbol name in 'Name'
    CHAR                        Name[1];                // symbol name (null terminated string)
} IMAGEHLP_SYMBOL64, *PIMAGEHLP_SYMBOL64;
typedef enum {
    AddrMode1616,
    AddrMode1632,
    AddrModeReal,
    AddrModeFlat
} ADDRESS_MODE;
typedef struct _tagADDRESS64 {
    DWORD64       Offset;
    WORD          Segment;
    ADDRESS_MODE  Mode;
} ADDRESS64, *LPADDRESS64;
typedef struct _KDHELP64 {
    DWORD64   Thread;
    DWORD   ThCallbackStack;
    DWORD   ThCallbackBStore;
    DWORD   NextCallback;
    DWORD   FramePointer;
    DWORD64   KiCallUserMode;
    DWORD64   KeUserCallbackDispatcher;
    DWORD64   SystemRangeStart;
    DWORD64  Reserved[8];
} KDHELP64, *PKDHELP64;
typedef struct _tagSTACKFRAME64 {
    ADDRESS64   AddrPC;               // program counter
    ADDRESS64   AddrReturn;           // return address
    ADDRESS64   AddrFrame;            // frame pointer
    ADDRESS64   AddrStack;            // stack pointer
    ADDRESS64   AddrBStore;           // backing store pointer
    PVOID       FuncTableEntry;       // pointer to pdata/fpo or NULL
    DWORD64     Params[4];            // possible arguments to the function
    BOOL        Far;                  // WOW far call
    BOOL        Virtual;              // is this a virtual frame?
    DWORD64     Reserved[3];
    KDHELP64    KdHelp;
} STACKFRAME64, *LPSTACKFRAME64;
typedef
BOOL
(__stdcall *PREAD_PROCESS_MEMORY_ROUTINE64)(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead
    );
typedef
PVOID
(__stdcall *PFUNCTION_TABLE_ACCESS_ROUTINE64)(
    HANDLE  hProcess,
    DWORD64 AddrBase
    );
typedef
DWORD64
(__stdcall *PGET_MODULE_BASE_ROUTINE64)(
    HANDLE  hProcess,
    DWORD64 Address
    );
typedef
DWORD64
(__stdcall *PTRANSLATE_ADDRESS_ROUTINE64)(
    HANDLE    hProcess,
    HANDLE    hThread,
    LPADDRESS64 lpaddr
    );
#define SYMOPT_CASE_INSENSITIVE         0x00000001
#define SYMOPT_UNDNAME                  0x00000002
#define SYMOPT_DEFERRED_LOADS           0x00000004
#define SYMOPT_NO_CPP                   0x00000008
#define SYMOPT_LOAD_LINES               0x00000010
#define SYMOPT_OMAP_FIND_NEAREST        0x00000020
#define SYMOPT_LOAD_ANYTHING            0x00000040
#define SYMOPT_IGNORE_CVREC             0x00000080
#define SYMOPT_NO_UNQUALIFIED_LOADS     0x00000100
#define SYMOPT_FAIL_CRITICAL_ERRORS     0x00000200
#define SYMOPT_EXACT_SYMBOLS            0x00000400
#define SYMOPT_ALLOW_ABSOLUTE_SYMBOLS   0x00000800
#define SYMOPT_IGNORE_NT_SYMPATH        0x00001000
#define SYMOPT_INCLUDE_32BIT_MODULES    0x00002000
#define SYMOPT_PUBLICS_ONLY             0x00004000
#define SYMOPT_NO_PUBLICS               0x00008000
#define SYMOPT_AUTO_PUBLICS             0x00010000
#define SYMOPT_NO_IMAGE_SEARCH          0x00020000
#define SYMOPT_SECURE                   0x00040000
#define SYMOPT_DEBUG                    0x80000000
#define UNDNAME_COMPLETE                 (0x0000)  // Enable full undecoration
#define UNDNAME_NAME_ONLY                (0x1000)  // Crack only the name for primary declaration;
#endif  // _MSC_VER < 1300

// Some missing defines (for VC5/6):
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif  


// secure-CRT_functions are only available starting with VC8
#if _MSC_VER < 1400
#define strcpy_s strcpy
#define strcat_s(dst, len, src) strcat(dst, src)
#define _snprintf_s _snprintf
#define _tcscat_s _tcscat
#endif

// Normally it should be enough to use 'CONTEXT_FULL' (better would be 'CONTEXT_ALL')
#define USED_CONTEXT_FLAGS CONTEXT_FULL


class StackWalkerInternal
{
public:
  StackWalkerInternal(AStackWalker *parent, HANDLE hProcess)
  {
    m_parent = parent;
    m_hDbhHelp = NULL;
    pSC = NULL;
    m_hProcess = hProcess;
    m_szSymPath = NULL;
  }
  ~StackWalkerInternal()
  {
    if (pSC != NULL)
      pSC(m_hProcess);  // SymCleanup
    if (m_hDbhHelp != NULL)
      FreeLibrary(m_hDbhHelp);
    m_hDbhHelp = NULL;
    m_parent = NULL;
    if(m_szSymPath != NULL)
      free(m_szSymPath);
    m_szSymPath = NULL;
  }
  BOOL Init(LPCSTR szSymPath)
  {
    // Dynamically load the Entry-Points for dbghelp.dll:
    // First try to load the newsest one from
    TCHAR szTemp[4096];
    // But before wqe do this, we first check if the ".local" file exists
    if (GetModuleFileName(NULL, szTemp, 4096) > 0)
    {
      _tcscat_s(szTemp, _T(".local"));
      if (GetFileAttributes(szTemp) == INVALID_FILE_ATTRIBUTES)
      {
        // ".local" file does not exist, so we can try to load the dbghelp.dll from the "Debugging Tools for Windows"
        if (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0)
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
          // Still not found? Then try to load the 64-Bit version:
        if ( (m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0) )
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows 64-Bit\\dbghelp.dll"));
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
      }
    }
    if (m_hDbhHelp == NULL)  // if not already loaded, try to load a default-one
      m_hDbhHelp = LoadLibrary( _T("dbghelp.dll") );
    if (m_hDbhHelp == NULL)
      return FALSE;
    pSI = (tSI) GetProcAddress(m_hDbhHelp, "SymInitialize" );
    pSC = (tSC) GetProcAddress(m_hDbhHelp, "SymCleanup" );

    pSW = (tSW) GetProcAddress(m_hDbhHelp, "StackWalk64" );
    pSGO = (tSGO) GetProcAddress(m_hDbhHelp, "SymGetOptions" );
    pSSO = (tSSO) GetProcAddress(m_hDbhHelp, "SymSetOptions" );

    pSFTA = (tSFTA) GetProcAddress(m_hDbhHelp, "SymFunctionTableAccess64" );
    pSGLFA = (tSGLFA) GetProcAddress(m_hDbhHelp, "SymGetLineFromAddr64" );
    pSGMB = (tSGMB) GetProcAddress(m_hDbhHelp, "SymGetModuleBase64" );
    pSGMI = (tSGMI) GetProcAddress(m_hDbhHelp, "SymGetModuleInfo64" );
    //pSGMI_V3 = (tSGMI_V3) GetProcAddress(m_hDbhHelp, "SymGetModuleInfo64" );
    pSGMI_V2 = (tSGMI_V2) GetProcAddress(m_hDbhHelp, "SymGetModuleInfo64" );
    pSGSFA = (tSGSFA) GetProcAddress(m_hDbhHelp, "SymGetSymFromAddr64" );
    pUDSN = (tUDSN) GetProcAddress(m_hDbhHelp, "UnDecorateSymbolName" );
    pSLM = (tSLM) GetProcAddress(m_hDbhHelp, "SymLoadModule64" );
    pSGSP =(tSGSP) GetProcAddress(m_hDbhHelp, "SymGetSearchPath" );

    if ( pSC == NULL || pSFTA == NULL || pSGMB == NULL || pSGMI == NULL ||
      pSGO == NULL || pSGSFA == NULL || pSI == NULL || pSSO == NULL ||
      pSW == NULL || pUDSN == NULL || pSLM == NULL )
    {
      FreeLibrary(m_hDbhHelp);
      m_hDbhHelp = NULL;
      pSC = NULL;
      return FALSE;
    }

    // SymInitialize
    if (szSymPath != NULL)
      m_szSymPath = _strdup(szSymPath);
    if (pSI(m_hProcess, m_szSymPath, FALSE) == FALSE)
    {  
      if (m_parent->m_options & AStackWalker::SWO_SHOW_LOOKUP_ERRORS)
        m_parent->OnDbgHelpErr("SymInitialize", GetLastError(), 0);
    }  
    
    DWORD symOptions = pSGO();  // SymGetOptions
    symOptions |= SYMOPT_LOAD_LINES;
    symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
    //symOptions |= SYMOPT_NO_PROMPTS;
    symOptions &= ~SYMOPT_UNDNAME;
    symOptions &= ~SYMOPT_DEFERRED_LOADS;
    // SymSetOptions
    symOptions = pSSO(symOptions);

    char buf[AStackWalker::STACKWALK_MAX_NAMELEN] = {0};
    if (pSGSP != NULL)
    {
      if (pSGSP(m_hProcess, buf, AStackWalker::STACKWALK_MAX_NAMELEN) == FALSE)
      {
        if (m_parent->m_options & AStackWalker::SWO_SHOW_LOOKUP_ERRORS)
          m_parent->OnDbgHelpErr("SymGetSearchPath", GetLastError(), 0);
      }
    }
    DWORD dwSize = 1024;
    AString userName(dwSize, 256);
    GetUserNameA(userName.startUsingCharPtr(dwSize), &dwSize);
    userName.stopUsingCharPtr(dwSize);
    m_parent->OnSymInit(buf, symOptions, userName);

    return TRUE;
  }

  AStackWalker *m_parent;

  HMODULE m_hDbhHelp;
  HANDLE m_hProcess;
  LPSTR m_szSymPath;

/*typedef struct IMAGEHLP_MODULE64_V3 {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symbol table
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    // new elements: 07-Jun-2002
    CHAR     LoadedImageName[256];   // symbol file name
    CHAR     LoadedPdbName[256];     // pdb file name
    DWORD    CVSig;                  // Signature of the CV record in the debug directories
    CHAR         CVData[MAX_PATH * 3];   // Contents of the CV record
    DWORD    PdbSig;                 // Signature of PDB
    GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
    DWORD    PdbAge;                 // DBI age of pdb
    BOOL     PdbUnmatched;           // loaded an unmatched pdb
    BOOL     DbgUnmatched;           // loaded an unmatched dbg
    BOOL     LineNumbers;            // we have line number information
    BOOL     GlobalSymbols;          // we have internal symbol information
    BOOL     TypeInfo;               // we have type information
    // new elements: 17-Dec-2003
    BOOL     SourceIndexed;          // pdb supports source server
    BOOL     Publics;                // contains public symbols
};
*/
typedef struct IMAGEHLP_MODULE64_V2 {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symbol table
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    CHAR     LoadedImageName[256];   // symbol file name
};


  // SymCleanup()
  typedef BOOL (__stdcall *tSC)( IN HANDLE hProcess );
  tSC pSC;

  // SymFunctionTableAccess64()
  typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD64 AddrBase );
  tSFTA pSFTA;

  // SymGetLineFromAddr64()
  typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
    OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );
  tSGLFA pSGLFA;

  // SymGetModuleBase64()
  typedef DWORD64 (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD64 dwAddr );
  tSGMB pSGMB;

  // SymGetModuleInfo64()
  typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PIMAGEHLP_MODULE64 ModuleInfo );
  tSGMI pSGMI;

//  // SymGetModuleInfo64()
//  typedef BOOL (__stdcall *tSGMI_V3)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT IMAGEHLP_MODULE64_V3 *ModuleInfo );
//  tSGMI_V3 pSGMI_V3;

  // SymGetModuleInfo64()
  typedef BOOL (__stdcall *tSGMI_V2)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT IMAGEHLP_MODULE64_V2 *ModuleInfo );
  tSGMI_V2 pSGMI_V2;

  // SymGetOptions()
  typedef DWORD (__stdcall *tSGO)( VOID );
  tSGO pSGO;

  // SymGetSymFromAddr64()
  typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
    OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol );
  tSGSFA pSGSFA;

  // SymInitialize()
  typedef BOOL (__stdcall *tSI)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
  tSI pSI;

  // SymLoadModule64()
  typedef DWORD64 (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
    IN PSTR ImageName, IN PSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll );
  tSLM pSLM;

  // SymSetOptions()
  typedef DWORD (__stdcall *tSSO)( IN DWORD SymOptions );
  tSSO pSSO;

  // StackWalk64()
  typedef BOOL (__stdcall *tSW)( 
    DWORD MachineType, 
    HANDLE hProcess,
    HANDLE hThread, 
    LPSTACKFRAME64 StackFrame, 
    PVOID ContextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );
  tSW pSW;

  // UnDecorateSymbolName()
  typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,
    DWORD UndecoratedLength, DWORD Flags );
  tUDSN pUDSN;

  typedef BOOL (__stdcall WINAPI *tSGSP)(HANDLE hProcess, PSTR SearchPath, DWORD SearchPathLength);
  tSGSP pSGSP;


private:
  // **************************************** ToolHelp32 ************************
  #define MAX_MODULE_NAME32 255
  #define TH32CS_SNAPMODULE   0x00000008
  #pragma pack( push, 8 )
  typedef struct tagMODULEENTRY32
  {
      DWORD   dwSize;
      DWORD   th32ModuleID;       // This module
      DWORD   th32ProcessID;      // owning process
      DWORD   GlblcntUsage;       // Global usage count on the module
      DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
      BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
      DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
      HMODULE hModule;            // The hModule of this module in th32ProcessID's context
      char    szModule[MAX_MODULE_NAME32 + 1];
      char    szExePath[MAX_PATH];
  } MODULEENTRY32;
  typedef MODULEENTRY32 *  PMODULEENTRY32;
  typedef MODULEENTRY32 *  LPMODULEENTRY32;
  #pragma pack( pop )

  BOOL GetModuleListTH32(HANDLE hProcess, DWORD pid)
  {
    // CreateToolhelp32Snapshot()
    typedef HANDLE (__stdcall *tCT32S)(DWORD dwFlags, DWORD th32ProcessID);
    // Module32First()
    typedef BOOL (__stdcall *tM32F)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
    // Module32Next()
    typedef BOOL (__stdcall *tM32N)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

    // try both dllstackframe...
    const TCHAR *dllname[] = { _T("kernel32.dll"), _T("tlhelp32.dll") };
    HINSTANCE hToolhelp = NULL;
    tCT32S pCT32S = NULL;
    tM32F pM32F = NULL;
    tM32N pM32N = NULL;

    HANDLE hSnap;
    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    BOOL keepGoing;
    int i;

    for (i = 0; i<(sizeof(dllname) / sizeof(dllname[0])); i++ )
    {
      hToolhelp = LoadLibrary( dllname[i] );
      if (hToolhelp == NULL)
        continue;
      pCT32S = (tCT32S) GetProcAddress(hToolhelp, "CreateToolhelp32Snapshot");
      pM32F = (tM32F) GetProcAddress(hToolhelp, "Module32First");
      pM32N = (tM32N) GetProcAddress(hToolhelp, "Module32Next");
      if ( (pCT32S != NULL) && (pM32F != NULL) && (pM32N != NULL) )
        break; // found the functions!
      FreeLibrary(hToolhelp);
      hToolhelp = NULL;
    }

    if (hToolhelp == NULL)
      return FALSE;

    hSnap = pCT32S( TH32CS_SNAPMODULE, pid );
    if (hSnap == (HANDLE) -1)
      return FALSE;

    keepGoing = pM32F( hSnap, &me );
    int cnt = 0;
    while (keepGoing)
    {
      LoadModule(hProcess, me.szExePath, me.szModule, (DWORD64) me.modBaseAddr, me.modBaseSize);
      ++cnt;
      keepGoing = pM32N( hSnap, &me );
    }
    CloseHandle(hSnap);
    FreeLibrary(hToolhelp);
    if (cnt <= 0)
      return FALSE;
    return TRUE;
  }  // GetModuleListTH32

  // **************************************** PSAPI ************************
  typedef struct _MODULEINFO {
      LPVOID lpBaseOfDll;
      DWORD SizeOfImage;
      LPVOID EntryPoint;
  } MODULEINFO, *LPMODULEINFO;

  BOOL GetModuleListPSAPI(HANDLE hProcess, DWORD pid)
  {
    // EnumProcessModules()
    typedef BOOL (__stdcall *tEPM)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
    // GetModuleFileNameEx()
    typedef DWORD (__stdcall *tGMFNE)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
    // GetModuleBaseName()
    typedef DWORD (__stdcall *tGMBN)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
    // GetModuleInformation()
    typedef BOOL (__stdcall *tGMI)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );

    HINSTANCE hPsapi;
    tEPM pEPM;
    tGMFNE pGMFNE;
    tGMBN pGMBN;
    tGMI pGMI;

    DWORD i;
    //ModuleEntry e;
    DWORD cbNeeded;
    MODULEINFO mi;
    HMODULE *hMods = 0;
    char *tt = NULL;
    char *tt2 = NULL;
    const SIZE_T TTBUFLEN = 8096;
    int cnt = 0;

    hPsapi = LoadLibrary( _T("psapi.dll") );
    if (hPsapi == NULL)
      return FALSE;

    pEPM = (tEPM) GetProcAddress( hPsapi, "EnumProcessModules" );
    pGMFNE = (tGMFNE) GetProcAddress( hPsapi, "GetModuleFileNameExA" );
    pGMBN = (tGMFNE) GetProcAddress( hPsapi, "GetModuleBaseNameA" );
    pGMI = (tGMI) GetProcAddress( hPsapi, "GetModuleInformation" );
    if ( (pEPM == NULL) || (pGMFNE == NULL) || (pGMBN == NULL) || (pGMI == NULL) )
    {
      // we couldn´t find all functions
      FreeLibrary(hPsapi);
      return FALSE;
    }

    hMods = (HMODULE*) malloc(sizeof(HMODULE) * (TTBUFLEN / sizeof HMODULE));
    tt = (char*) malloc(sizeof(char) * TTBUFLEN);
    tt2 = (char*) malloc(sizeof(char) * TTBUFLEN);
    if (!hMods || !tt || !tt2)
      return FALSE;
    
    if ( ! pEPM( hProcess, hMods, TTBUFLEN, &cbNeeded ) )
    {
      //_ftprintf(fLogFile, _T("%lu: EPM failed, GetLastError = %lu\n"), g_dwShowCount, gle );
      goto cleanup;
    }

    if ( cbNeeded > TTBUFLEN )
    {
      //_ftprintf(fLogFile, _T("%lu: More than %lu module handlestackframe. Huh?\n"), g_dwShowCount, lenof( hMods ) );
      goto cleanup;
    }

    for ( i = 0; i < cbNeeded / sizeof hMods[0]; i++ )
    {
      // base address, size
      pGMI(hProcess, hMods[i], &mi, sizeof mi );
      // image file name
      tt[0] = 0;
      pGMFNE(hProcess, hMods[i], tt, TTBUFLEN );
      // module name
      tt2[0] = 0;
      pGMBN(hProcess, hMods[i], tt2, TTBUFLEN );

      LoadModule(hProcess, tt, tt2, (DWORD64) mi.lpBaseOfDll, mi.SizeOfImage);
      cnt++;
    }

  cleanup:
    if (hPsapi)
      FreeLibrary(hPsapi);
    free(tt2);
    free(tt);
    free(hMods);

    return cnt != 0;
  }  // GetModuleListPSAPI

  void LoadModule(HANDLE hProcess, LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size)
  {
    CHAR *i = _strdup(img);
    CHAR *m = _strdup(mod);
    DWORD result = ERROR_SUCCESS;
    if (pSLM(hProcess, 0, i, m, baseAddr, size) == 0)
      result = GetLastError();
    ULONGLONG fileVersion = 0;
    if (m_parent != NULL)
    {
      // try to retrive the file-version:
      if ( (m_parent->m_options & AStackWalker::SWO_FILE_VERSION) != 0)
      {
        LPVOID vData = NULL;
        VS_FIXEDFILEINFO *fInfo = NULL;
        DWORD dwHandle;
        // Fix for VC7.0-PSDK (GetFileVersionInfoSize is wrongly defined)
        LPSTR szImg = _strdup(img);
        DWORD dwSize = GetFileVersionInfoSizeA(szImg, &dwHandle);
        if (dwSize > 0)
        {
          vData = malloc(dwSize);
          if (vData && GetFileVersionInfoA(szImg, dwHandle, dwSize, vData) != 0)
          {
            UINT len;
            if (VerQueryValue(vData, _T("\\"), (LPVOID*) &fInfo, &len) == 0)
              fInfo = NULL;
            else
            {
              fileVersion = ((ULONGLONG)fInfo->dwFileVersionLS) + ((ULONGLONG)fInfo->dwFileVersionMS << 32);
            }
          }
        }
        if (szImg != NULL)
          free(szImg);
        if (vData != NULL)
          free(vData);
      }

      // Retrive some additional-infos about the module
      IMAGEHLP_MODULE64_V2 Module;
      GetModuleInfo(hProcess, baseAddr, &Module);
      const char *szSymType = "-unknown-";
      switch(Module.SymType)
      {
        case SymNone:
          szSymType = "-nosymbols-";
          break;
        case SymCoff:
          szSymType = "COFF";
          break;
        case SymCv:
          szSymType = "CV";
          break;
        case SymPdb:
          szSymType = "PDB";
          break;
        case SymExport:
          szSymType = "-exported-";
          break;
        case SymDeferred:
          szSymType = "-deferred-";
          break;
        case SymSym:
          szSymType = "SYM";
          break;
#if API_VERSION_NUMBER >= 9
        case SymDia:
          szSymType = "DIA";
          break;
#endif
        case 8: //SymVirtual:
          szSymType = "Virtual";
          break;
      }
      m_parent->OnLoadModule(img, mod, baseAddr, size, result, szSymType, Module.LoadedImageName, fileVersion);
    }
    free(i);
    free(m);
  }
public:
  BOOL LoadModules(HANDLE hProcess, DWORD dwProcessId)
  {
    // first try toolhelp32
    if (GetModuleListTH32(hProcess, dwProcessId))
      return true;
    // then try psapi
    return GetModuleListPSAPI(hProcess, dwProcessId);
  }


  BOOL GetModuleInfo(HANDLE hProcess, DWORD64 baseAddr, IMAGEHLP_MODULE64_V2 *pModuleInfo)
  {
    if(pSGMI_V2 == NULL)
    {
      SetLastError(ERROR_DLL_INIT_FAILED);
      return FALSE;
    }
    // First try to use the larger ModuleInfo-Structure
//    memset(pModuleInfo, 0, sizeof(IMAGEHLP_MODULE64_V3));
//    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V3);
//    if (pSGMI_V3 != NULL)
//    {
//      if (pSGMI_V3(hProcess, baseAddr, pModuleInfo) != FALSE)
//        return TRUE;
//      // check if the parameter was wrong (size is bad...)
//      if (GetLastError() != ERROR_INVALID_PARAMETER)
//        return FALSE;
//    }
    // could not retrive the bigger structure, try with the smaller one (as defined in VC7.1)...
    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
    void *pData = malloc(4096); // reserve enough memory, so the bug in v6.3.5.1 does not lead to memory-overwritestackframe...
    memcpy(pData, pModuleInfo, sizeof(IMAGEHLP_MODULE64_V2));
    if (pSGMI_V2 != NULL)
    {
      if (pSGMI_V2(hProcess, baseAddr, (IMAGEHLP_MODULE64_V2*) pData) != FALSE)
      {
        // only copy as much memory as is reserved...
        memcpy(pModuleInfo, pData, sizeof(IMAGEHLP_MODULE64_V2));
        pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
        free(pData);
        return TRUE;
      }
      free(pData);
      return FALSE;
    }
    free(pData);
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }
};

// #############################################################
AStackWalker::AStackWalker(
  DWORD dwProcessId, 
  HANDLE hProcess
) :
  m_options(AStackWalker::SWO_SET_ALL),
  m_modulesLoaded(FALSE),
  m_hProcess(hProcess),
  m_dwProcessId(dwProcessId),
  mp_swi(NULL)
{
  if (m_dwProcessId == -1)
    m_dwProcessId = ::GetCurrentProcessId();

  if (!m_hProcess)
    m_hProcess = ::GetCurrentProcess();

  mp_swi = new StackWalkerInternal(this, m_hProcess);
}

AStackWalker::AStackWalker(
  u4 options, 
  const AString& symbolPath,   // = AConstant::ASTRING_EMPTY
  DWORD dwProcessId,           // = -1, 
  HANDLE hProcess              // = NULL
) :
  m_options(options),
  m_modulesLoaded(FALSE),
  m_hProcess(hProcess),
  m_dwProcessId(dwProcessId),
  mp_swi(NULL),
  m_symbolPath(symbolPath)
{
  if (m_dwProcessId == -1)
    m_dwProcessId = ::GetCurrentProcessId();

  if (!m_hProcess)
    m_hProcess = ::GetCurrentProcess();
  
  mp_swi = new StackWalkerInternal(this, m_hProcess);
  if (!m_symbolPath.isEmpty())
    m_options |= AStackWalker::SWO_SYM_BUILDPATH;
}

AStackWalker::AStackWalker(const AStackWalker& that)
{
  if (this == &that)
    return;
  else
  {
    mp_swi = NULL;
    operator=(that);
  }
}

AStackWalker& AStackWalker::operator=(const AStackWalker& that)
{
  if (this == &that) return *this;

  m_options = that.m_options;
  m_modulesLoaded = that.m_modulesLoaded;
  m_hProcess = that.m_hProcess;
  m_dwProcessId = that.m_dwProcessId;
  m_symbolPath = that.m_symbolPath;

  //a_The stack has been already walked by that, only copy of stack data
  if (!mp_swi)
    mp_swi = new StackWalkerInternal(this, m_hProcess);

  _clearStackWalkResult();

  STACKLINES::const_iterator cit = that.m_StackWalkResult.begin();
  while (cit != that.m_StackWalkResult.end())
  {
    m_StackWalkResult.push_back(new ARope(*(*cit)));
    ++cit;
  }

  return *this;
}


AStackWalker::~AStackWalker()
{
  try
  {
    _clearStackWalkResult();
    delete mp_swi;
  }
  catch(...) {}
}

void AStackWalker::_clearStackWalkResult()
{
  if (m_StackWalkResult.size() > 0)
  {
    STACKLINES::iterator it = m_StackWalkResult.begin();
    while (it != m_StackWalkResult.end())
    {
      delete *it;
      ++it;
    }
    m_StackWalkResult.clear();
  }
}

BOOL AStackWalker::LoadModules()
{
  if (!mp_swi)
  {
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }
  if (m_modulesLoaded != FALSE)
    return TRUE;

  // Build the sym-path:
#pragma message("AStackWalker::LoadModules(): FIXME!")
  char *szSymPath = NULL;
  if ( (m_options & AStackWalker::SWO_SYM_BUILDPATH) != 0)
  {
    const size_t nSymPathLen = 4096;
    szSymPath = (char*) malloc(nSymPathLen);
    szSymPath[0] = 0;
    // Now first add the (optional) provided sympath:
    if (!m_symbolPath.isEmpty())
    {
      strcat_s(szSymPath, nSymPathLen, m_symbolPath.c_str());
      strcat_s(szSymPath, nSymPathLen, ";");
    }

    strcat_s(szSymPath, nSymPathLen, ".;");

    const size_t nTempLen = 1024;
    char szTemp[nTempLen];
    // Now add the current directory:
    if (GetCurrentDirectoryA(nTempLen, szTemp) > 0)
    {
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }

    // Now add the path for the main-module:
    if (GetModuleFileNameA(NULL, szTemp, nTempLen) > 0)
    {
      for (char *p = (szTemp+strlen(szTemp)-1); p >= szTemp; --p)
      {
        // locate the rightmost path separator
        if ( (*p == '\\') || (*p == '/') || (*p == ':') )
        {
          *p = 0;
          break;
        }
      }  // for (search for path separator...)
      if (strlen(szTemp) > 0)
      {
        strcat_s(szSymPath, nSymPathLen, szTemp);
        strcat_s(szSymPath, nSymPathLen, ";");
      }
    }
    if (GetEnvironmentVariableA("_NT_SYMBOL_PATH", szTemp, nTempLen) > 0)
    {
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }
    if (GetEnvironmentVariableA("_NT_ALTERNATE_SYMBOL_PATH", szTemp, nTempLen) > 0)
    {
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }
    if (GetEnvironmentVariableA("SYSTEMROOT", szTemp, nTempLen) > 0)
    {
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
      // also add the "system32"-directory:
      strcat_s(szTemp, nTempLen, "\\system32");
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }

    if ( (m_options & AStackWalker::SWO_SYM_BUILDPATH) != 0)
    {
      if (GetEnvironmentVariableA("SYSTEMDRIVE", szTemp, nTempLen) > 0)
      {
        strcat_s(szSymPath, nSymPathLen, "SRV*");
        strcat_s(szSymPath, nSymPathLen, szTemp);
        strcat_s(szSymPath, nSymPathLen, "\\websymbols");
        strcat_s(szSymPath, nSymPathLen, "*http://msdl.microsoft.com/download/symbols;");
      }
      else
        strcat_s(szSymPath, nSymPathLen, "SRV*c:\\websymbols*http://msdl.microsoft.com/download/symbols;");
    }
  }

  // First Init the whole stuff...
  BOOL bRet = mp_swi->Init(szSymPath);
  if (szSymPath != NULL) free(szSymPath); szSymPath = NULL;
  if (bRet == FALSE)
  {
    OnDbgHelpErr("Error while initializing dbghelp.dll", 0, 0);
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }

  bRet = mp_swi->LoadModules(m_hProcess, m_dwProcessId);
  if (bRet != FALSE)
    m_modulesLoaded = TRUE;
  return bRet;
}


// The following is used to pass the "userData"-Pointer to the user-provided readMemoryFunction
// This has to be done due to a problem with the "hProcess"-parameter in x64...
// Because this class is in no case multi-threading-enabled (because of the limitations 
// of dbghelp.dll) it is "safe" to use a static-variable
static AStackWalker::PReadProcessMemoryRoutine s_readMemoryFunction = NULL;
static LPVOID s_readMemoryFunction_UserData = NULL;

BOOL AStackWalker::ProcessCallstack(
  bool skipCallingFrame,                         // = false
  HANDLE hThread,                                // = NULL
  CONTEXT *pContext,                             // = NULL
  PReadProcessMemoryRoutine readMemoryFunction,  // = NULL
  LPVOID pUserData                               // = NULL
)
{
  ALock lock(sm_StackWalkLock);

  //a_Clear the results since we are walking again
  _clearStackWalkResult();

  if (!hThread)
    hThread = ::GetCurrentThread();

  CONTEXT context;
  CallstackEntry csEntry;
  StackWalkerInternal::IMAGEHLP_MODULE64_V2 Module;
  IMAGEHLP_LINE64 Line;
  int frameNum;

  if (m_modulesLoaded == FALSE)
    LoadModules();  // ignore the result...

  if (mp_swi->m_hDbhHelp == NULL)
  {
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }

  s_readMemoryFunction = readMemoryFunction;
  s_readMemoryFunction_UserData = pUserData;

  if (!pContext)
  {
    // If no context is provided, capture the context
    if (hThread == GetCurrentThread())
    {
      GET_CURRENT_CONTEXT(context, USED_CONTEXT_FLAGS);
    }
    else
    {
      SuspendThread(hThread);
      memset(&context, 0, sizeof(CONTEXT));
      context.ContextFlags = USED_CONTEXT_FLAGS;
      if (GetThreadContext(hThread, &context) == FALSE)
      {
        ResumeThread(hThread);
        return FALSE;
      }
    }
  }
  else
    context = *pContext;

  // init STACKFRAME for first call
  STACKFRAME64 stackframe; // in/out stackframe
  memset(&stackframe, 0, sizeof(STACKFRAME64));
  DWORD imageType;
#ifdef _M_IX86
  // normally, call ImageNtHeader() and use machine info from PE header
  imageType = IMAGE_FILE_MACHINE_I386;
  stackframe.AddrPC.Offset = context.Eip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Ebp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Esp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  imageType = IMAGE_FILE_MACHINE_AMD64;
  stackframe.AddrPC.Offset = context.Rip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Rsp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Rsp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  imageType = IMAGE_FILE_MACHINE_IA64;
  stackframe.AddrPC.Offset = context.StIIP;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.IntSp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrBStore.Offset = context.RsBSP;
  stackframe.AddrBStore.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.IntSp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

  //IMAGEHLP_SYMBOL64 *pSym = NULL;
  //pSym = (IMAGEHLP_SYMBOL64 *) malloc(sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
  //if (!pSym) goto cleanup;  // not enough memory...
  //memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
  //pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
  //pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

  //u1 buffer[sizeof(SYMBOL_INFO)+STACKWALK_MAX_NAMELEN+1];
  //u1 debugpad[128] = {0,0,0,0,0,0,0,0,0,0,0,0};
  //SYMBOL_INFO *pSymbol = (SYMBOL_INFO *)((void *)buffer);
  //pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  //pSymbol->MaxNameLen = STACKWALK_MAX_NAMELEN;

  u1 _internal_buffer[sizeof(IMAGEHLP_SYMBOL64)+STACKWALK_MAX_NAMELEN+1];
  IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64 *)((void *)_internal_buffer);
  pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
  pSymbol->MaxNameLength = STACKWALK_MAX_NAMELEN;

  memset(&Line, 0, sizeof(Line));
  Line.SizeOfStruct = sizeof(Line);

  memset(&Module, 0, sizeof(Module));
  Module.SizeOfStruct = sizeof(Module);

  for (frameNum = 0; ; ++frameNum )
  {
    // get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
    // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
    // assume that either you are done, or that the stack is so hosed that the next
    // deeper frame could not be found.
    // CONTEXT need not to be suplied if imageTyp is IMAGE_FILE_MACHINE_I386!
    if (!mp_swi->pSW(imageType, m_hProcess, hThread, &stackframe, &context, myReadProcMem, mp_swi->pSFTA, mp_swi->pSGMB, NULL))
    {
      if (m_options & AStackWalker::SWO_SHOW_LOOKUP_ERRORS)
        OnDbgHelpErr("StackWalk64", GetLastError(), stackframe.AddrPC.Offset);
      break;
    }

    //a_Ignore the AStackWalker frame and calling frame if specified (AException uses true so that actual spot of exception is shown and not ctor of exception)
    if (0 == frameNum || (skipCallingFrame && 1 == frameNum))
      continue;

    csEntry.offset = stackframe.AddrPC.Offset;
    csEntry.offsetFromSymbol = 0;
    csEntry.offsetFromLine = 0;
    csEntry.lineNumber = 0;
    if ( stackframe.AddrPC.Offset != 0 )
    {
      // we seem to have a valid PC
      // show procedure info (SymGetSymFromAddr64())
      if (mp_swi->pSGSFA(m_hProcess, stackframe.AddrPC.Offset, &(csEntry.offsetFromSymbol), pSymbol))
      {
        csEntry.name.assign(pSymbol->Name);
        
        // UnDecorateSymbolName()
        u4 ret = mp_swi->pUDSN(pSymbol->Name, csEntry.undName.startUsingCharPtr(STACKWALK_MAX_NAMELEN), STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY );
        if (ret)
          csEntry.undName.stopUsingCharPtr(ret);
        else
          csEntry.undName.stopUsingCharPtr(0);
          
        ret = mp_swi->pUDSN(pSymbol->Name, csEntry.undFullName.startUsingCharPtr(STACKWALK_MAX_NAMELEN), STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE );
        if (ret)
          csEntry.undFullName.stopUsingCharPtr(ret);
        else
          csEntry.undFullName.stopUsingCharPtr(0);
      }
      else
      {
        if (m_options & AStackWalker::SWO_SHOW_LOOKUP_ERRORS)
          OnDbgHelpErr("SymGetSymFromAddr64", GetLastError(), stackframe.AddrPC.Offset);
      }

      // show line number info, NT5.0-method (SymGetLineFromAddr64())
      if (mp_swi->pSGLFA != NULL )
      { // yes, we have SymGetLineFromAddr64()
        if (mp_swi->pSGLFA(m_hProcess, stackframe.AddrPC.Offset, &(csEntry.offsetFromLine), &Line) != FALSE)
        {
          csEntry.lineNumber = Line.LineNumber;
          csEntry.lineFileName.assign(Line.FileName);
        }
        else
        {
          if (m_options & AStackWalker::SWO_SHOW_LOOKUP_ERRORS)
            OnDbgHelpErr("SymGetLineFromAddr64", GetLastError(), stackframe.AddrPC.Offset);
        }
      } // yes, we have SymGetLineFromAddr64()

      // show module info (SymGetModuleInfo64())
      if (mp_swi->GetModuleInfo(m_hProcess, stackframe.AddrPC.Offset, &Module ) != FALSE)
      { // got module info OK
        switch ( Module.SymType )
        {
        case SymNone:
          csEntry.symTypeString = "-nosymbols-";
          break;
        case SymCoff:
          csEntry.symTypeString = "COFF";
          break;
        case SymCv:
          csEntry.symTypeString = "CV";
          break;
        case SymPdb:
          csEntry.symTypeString = "PDB";
          break;
        case SymExport:
          csEntry.symTypeString = "-exported-";
          break;
        case SymDeferred:
          csEntry.symTypeString = "-deferred-";
          break;
        case SymSym:
          csEntry.symTypeString = "SYM";
          break;
#if API_VERSION_NUMBER >= 9
        case SymDia:
          csEntry.symTypeString = "DIA";
          break;
#endif
        default:
          //_snprintf( ty, sizeof ty, "symtype=%ld", (long) Module.SymType );
          csEntry.symTypeString = NULL;
          break;
        }

        csEntry.moduleName.assign(Module.ModuleName);
        csEntry.baseOfImage = Module.BaseOfImage;
        csEntry.loadedImageName.assign(Module.LoadedImageName);
      } // got module info OK
      else
      {
        OnDbgHelpErr("SymGetModuleInfo64", GetLastError(), stackframe.AddrPC.Offset);
      }
    } // we seem to have a valid PC

    CallstackEntryType et = nextEntry;
    if (frameNum == 0)
      et = firstEntry;
    OnCallstackEntry(et, csEntry);
    
    if (stackframe.AddrReturn.Offset == 0)
    {
      OnCallstackEntry(lastEntry, csEntry);
      SetLastError(ERROR_SUCCESS);
      break;
    }
  } // for ( frameNum )

  //cleanup:
  //  if (pSym) free( pSym );

  if (!pContext)
    ResumeThread(hThread);

  return TRUE;
}

BOOL __stdcall AStackWalker::myReadProcMem(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead
    )
{
  if (s_readMemoryFunction == NULL)
  {
    SIZE_T st;
    BOOL bRet = ReadProcessMemory(hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, &st);
    *lpNumberOfBytesRead = (DWORD) st;
    //printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, (DWORD) st, (DWORD) bRet);
    return bRet;
  }
  else
  {
    return s_readMemoryFunction(hProcess, qwBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead, s_readMemoryFunction_UserData);
  }
}

void AStackWalker::OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion)
{
  if (m_options & AStackWalker::SWO_SYM_ALL)
  {
    AAutoPtr<ARope> pRope(new ARope());

    pRope->append(img);
    pRope->append(':');
    pRope->append(mod);
    pRope->append(" (0x");
    pRope->append(AString::fromPointer((LPVOID) baseAddr));
    pRope->append(") size=");
    pRope->append(AString::fromU4(size));
    pRope->append(" result=");
    pRope->append(AString::fromU4(result));
    pRope->append(" SymType=");
    pRope->append(symType);
    pRope->append(" PDB=");
    pRope->append(pdbName);
    pRope->append(" fileVer=0x");
    pRope->append(AString::fromU8(fileVersion, 0x10));

    m_StackWalkResult.push_back(pRope);
    pRope.setOwnership(false);
  }
}

void AStackWalker::OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
{
  if ((eType != lastEntry) && (entry.offset != 0))
  {
    AAutoPtr<ARope> pRope(new ARope());
    
    //a_Module name
    if (!entry.moduleName.isEmpty())
    {
      pRope->append(entry.moduleName);
      pRope->append(":::",3);
    }

    //a_Function name
    if (!entry.name.isEmpty())
      pRope->append(entry.name);
    else if (!entry.undName.isEmpty())
      pRope->append(entry.undName);
    else if (!entry.undFullName.isEmpty())
      pRope->append(entry.undFullName);
    
    //a_Filename
    if (!entry.lineFileName.isEmpty())
    {
      pRope->append(" - ",3);
      pRope->append(entry.lineFileName);
      if (entry.lineNumber)
      {
        pRope->append(':');
        pRope->append(AString::fromU4(entry.lineNumber));
      }
    }
    
    pRope->append(" (");
    pRope->append(AString::fromPointer((LPVOID) entry.offset));
    pRope->append(')');

    m_StackWalkResult.push_back(pRope);
    pRope.setOwnership(false);
  }
}

void AStackWalker::OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
{
  AAutoPtr<ARope> pRope(new ARope());

  pRope->append("ERROR: ");
  pRope->append(szFuncName);
  pRope->append("  GetLastError=");
  pRope->append(AString::fromU4(gle));
  pRope->append("  Address=0x");
  pRope->append(AString::fromPointer((LPVOID) addr));

  m_StackWalkResult.push_back(pRope);
  pRope.setOwnership(false);
}

void AStackWalker::OnSymInit(LPCSTR szSearchPath, DWORD symOptions, const AString& userName)
{
  if (m_options & AStackWalker::SWO_INIT_INFO)
  {
    AAutoPtr<ARope> pRope(new ARope());

    OSVERSIONINFOEXA ver;
    ZeroMemory(&ver, sizeof(OSVERSIONINFOEXA));
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (GetVersionExA( (OSVERSIONINFOA*) &ver) != FALSE)
    {
      pRope->append("OS { version=");
      pRope->append(AString::fromU4(ver.dwMajorVersion));
      pRope->append('.');
      pRope->append(AString::fromU4(ver.dwMinorVersion));
      pRope->append(", build=");
      pRope->append(AString::fromU4(ver.dwBuildNumber));
      pRope->append(" (");
      pRope->append(ver.szCSDVersion);
      pRope->append(") suite=");
      pRope->append(AString::fromU2(ver.wSuiteMask));
      pRope->append(" product=");
      pRope->append(AString::fromU2(ver.wProductType));
      pRope->append("}  user=");
      pRope->append(userName);
    }

    if (m_options & AStackWalker::SWO_SYM_ALL)
    {
      pRope->append("  symSearchPath={");
      pRope->append(szSearchPath);
      pRope->append("}  symOptions: ");
      pRope->append(AString::fromU4(symOptions));
    }

    m_StackWalkResult.push_back(pRope);
    pRope.setOwnership(false);
  }
}

void AStackWalker::emit(AOutputBuffer& target) const
{
  STACKLINES::const_iterator cit = m_StackWalkResult.begin();
  while(cit != m_StackWalkResult.end())
  {
    target.append(*(*cit));
    target.append(AConstant::ASTRING_EOL);
    ++cit;
  }
}

void AStackWalker::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("AStackWalker", 12);

  STACKLINES::const_iterator cit = m_StackWalkResult.begin();
  while(cit != m_StackWalkResult.end())
  {
    target.addElement(ASW("stack",5), *(*cit), AXmlElement::ENC_CDATASAFE);
    ++cit;
  }
}
