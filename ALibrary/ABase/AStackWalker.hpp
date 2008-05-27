/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AStackWalker_HPP__
#define INCLUDED__AStackWalker_HPP__

#include "apiABase.hpp"
#include "AOutputBuffer.hpp"
#include "AEmittable.hpp"
#include "AXmlEmittable.hpp"
#include "AString.hpp"
#include "ARope.hpp"
#include "ASync_CriticalSection.hpp"

/*!
 StackWalker.hpp


 History:
  2005-07-27   v1 - First public release on http://www.codeproject.com/

  2006_Mar_29  by achacha
               Heavily modified to fit into ALibrary (naming/build structure changes)
               Fixed string usage to avoid using the dangerous sprintf in a stack walker
               Added ability to store result and emit as text or XML
               Cleaned up message format
*/

// special defines for VC5/6 (if no actual PSDK is installed):
#if _MSC_VER < 1300
typedef unsigned __int64 DWORD64, *PDWORD64;
#if defined(_WIN64)
typedef unsigned __int64 SIZE_T, *PSIZE_T;
#else
typedef unsigned long SIZE_T, *PSIZE_T;
#endif
#endif  // _MSC_VER < 1300

class ABASE_API StackWalkerInternal;  // forward
class ABASE_API AStackWalker : public AEmittable, public AXmlEmittable
{
public:
  typedef enum StackWalkOptions
  {
    SWO_NONE               = 0x0000,          // No addition info will be retrived (only the address is available)
    SWO_SYMBOLS            = 0x0001,          // Try to get the symbol-name
    SWO_LINE               = 0x0002,          // Try to get the line for this symbol
    SWO_MODULE_INFO        = 0x0004,          // Try to retrieve the module-infos
    SWO_FILE_VERSION       = 0x0008,          // Also retrieve the version for the DLL/EXE
                                     
    SWO_SYM_BUILDPATH      = 0x0010,          // Generate a "good" symbol-search-path
    SWO_SYM_USESYMSRV      = 0x0020,          // Also use the public Microsoft-Symbol-Server
    SWO_SYM_ALL            = 0x00f0,          //a_All symbol methods
    
    SWO_INIT_INFO          = 0x0100,          //a_Include init information
    
    SWO_SHOW_LOOKUP_ERRORS = 0x1000,          //a_To show errors during symbol lookup or not

    // Useful sets
    SWO_SET_STACKONLY      = 0x000f,          //a_Only the call stack, no errors
    SWO_SET_ALL            = 0xffff           // Contains all options (default)


  } StackWalkOptions;

  /*!
  Usage
  
  ARope rope;
  AStackWalker sw;
  sw.ProcessCallstack();
  sw.emit(rope);
  std::cerr << rope << std::endl;
  
  */
  AStackWalker(
    u4 options = AStackWalker::SWO_SET_ALL,
    const AString& symbolPath = AConstant::ASTRING_EMPTY, 
    DWORD dwProcessId = -1, 
    HANDLE hProcess = NULL
  );
  
  AStackWalker(
    DWORD dwProcessId, 
    HANDLE hProcess
  );
  
  /*!
  Process Callstack
  */
  typedef BOOL (__stdcall *PReadProcessMemoryRoutine)(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead,
    LPVOID      pUserData  // optional data, which was passed in "ShowCallstack"
  );

  /*!
  Synchronizes internally

  skipCallingFrame - will skip ProcessCallstack frame and if true will skip one more (often exception ctor)
  */
  BOOL ProcessCallstack(
    bool skipCallingFrame = false,
    HANDLE hThread = NULL, 
    CONTEXT *context = NULL, 
    PReadProcessMemoryRoutine readMemoryFunction = NULL,
    LPVOID pUserData = NULL  // optional to identify some data in the 'readMemoryFunction'-callback
  );

  /*!
  Copy ctors
  */
  AStackWalker(const AStackWalker&);
  AStackWalker& operator=(const AStackWalker&);

  virtual ~AStackWalker();

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  AXmlEmittable
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

////////////////////// INTERNAL STUFF BELOW THIS LINE/////////////////////////

  BOOL LoadModules();

#if _MSCVER >= 1300
// due to some reasons, the "STACKWALK_MAX_NAMELEN" must be declared as "public" 
// in older compilers in order to use it... starting with VC7 we can declare it as "protected"
protected:
#endif
	enum { STACKWALK_MAX_NAMELEN = 1024 }; // max name length for found symbols

protected:
  // Entry for each Callstack-Entry
  typedef struct CallstackEntry
  {
    DWORD64 offset;  // if 0, we have no valid entry
    AString name;
    AString undName;
    AString undFullName;
    DWORD64 offsetFromSymbol;
    DWORD offsetFromLine;
    DWORD lineNumber;
    AString lineFileName;
    DWORD symType;
    LPCSTR symTypeString;
    AString moduleName;
    DWORD64 baseOfImage;
    AString loadedImageName;
  } CallstackEntry;

  typedef enum CallstackEntryType {firstEntry, nextEntry, lastEntry};

  void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, const AString& userName);
  void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion);
  void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
  void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);

  StackWalkerInternal *mp_swi;
  HANDLE               m_hProcess;
  DWORD                m_dwProcessId;
  BOOL                 m_modulesLoaded;
  AString              m_symbolPath;
  u4 m_options;

  static BOOL __stdcall myReadProcMem(HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);

  friend StackWalkerInternal;

private:
  typedef std::list<ARope *> STACKLINES;
  STACKLINES m_StackWalkResult;

  static ASync_CriticalSection sm_StackWalkLock;

  void _clearStackWalkResult();  //a_Clear and deallocate stack walk results
};

#endif //INCLUDED__AStackWalker_HPP__
