/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ABase.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AString.hpp"
#include "ATime.hpp"
#include "templateMapOfPtrs.hpp"

#ifdef __WINDOWS__
#include "Dbghelp.h"
#endif

ABase::ABase() :
  pNext(NULL),
  pPrev(NULL)
{
}

ABase::~ABase()
{
}

void ABase::setNext(ABase *p)
{
  pNext = p;
}

void ABase::setPrev(ABase *p)
{
  pPrev = p;
}

const ABase *ABase::getNext() const
{
  return pNext;
}

const ABase *ABase::getPrev() const
{
  return pPrev;
}

ABase *ABase::useNext()
{
  return pNext;
}

ABase *ABase::usePrev()
{
  return pPrev;
}

ABase *ABase::unlink()
{
  if (pPrev)
    pPrev->pNext = pNext;

  if (pNext)
  {
    pNext->pPrev = pPrev;
    pNext = NULL;
  }

  pPrev = NULL;

  return this;
}

#ifdef DEBUG_TRACK_ABASE_MEMORY
#define MAX_STACKDATA_BUFFER 2048
#define MAX_STACKDATA_FILE_BUFFER 1024
class StackData
{
public:
  StackData() : line(0)
  {
    *undecorated = '\x0';
    *filename = '\x0';
  }
  char undecorated[MAX_STACKDATA_BUFFER];
  DWORD64 line;
  char filename[MAX_STACKDATA_BUFFER];

  void emit(AOutputBuffer& target)
  {
    target.append(undecorated);
    if (*filename)
    {
      target.append('(');
      target.append(filename);
      target.append(':');
      target.append(AString::fromU8(line));
      target.append(")\r\n",3);
    }
  }
};

// Allows easy emit on AllocData since it cannot be based on ABase

class AllocData
{
public:
  typedef std::list<StackData *> STACKFRAMES;

public:
  AllocData() : ptr(NULL), size(0), time(0) {}
  AllocData(const AllocData& that) : ptr(that.ptr), size(that.size), time(that.time) {}
  virtual ~AllocData()
  {
    std::cerr << "---" << time << " - 0x" << std::hex << ptr << std::dec << "(" << size << ")" << std::endl;
    for (STACKFRAMES::iterator it = stackframes.begin(); it != stackframes.end(); ++it)
    {
      std::cerr << "  " << (*it)->undecorated << ":" << (*it)->line << std::endl;
      delete (*it);
    }
  }

  virtual void emit(AOutputBuffer& target, bool includeDebugDump) const
  {
    target.append("t=",2);
    target.append(AString::fromU8(time));
    target.append(",p=",3);
    target.append(AString::fromPointer(ptr));
    target.append(",s=",3);
    target.append(AString::fromSize_t(size));

    STACKFRAMES::const_iterator it = stackframes.begin();
    if (it != stackframes.end())
      target.append(AConstant::ASTRING_EOL);
    for (; it != stackframes.end(); ++it)
    {
      target.append(AConstant::ASTRING_TWOSPACES);
      (*it)->emit(target);
    }
    if (includeDebugDump)
    {
      target.append(AConstant::ASTRING_EOL);
      ADebugDumpable *pDD = (ADebugDumpable *)(ptr);
      pDD->debugDumpToAOutputBuffer(target, 1);
    }
  }

  bool operator<(const AllocData& that) { return time < that.time; }
  bool operator>(const AllocData& that) { return time > that.time; }
  ABase *ptr;
  size_t size;
  u8 time;
  STACKFRAMES stackframes;
};
class AllocDataWrapper : public AEmittable
{
public:
  AllocDataWrapper(AllocData *p = NULL, bool full = false) : pData(p), fullDump(full) {}
  virtual void emit(AOutputBuffer& target) const { pData->emit(target, fullDump); }
private:
  AllocData *pData;
  bool fullDump;
};

static ASync_CriticalSection GLOBAL_mp_ConsoleSync;
typedef AMapOfPtrs<void *, AllocData> MAP_VOIDPTR_SIZET;
static MAP_VOIDPTR_SIZET GLOBAL_m_Allocations;

BOOL __stdcall myReadProcMem(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead
    )
{
  SIZE_T st;
  BOOL bRet = ReadProcessMemory(hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, &st);
  *lpNumberOfBytesRead = (DWORD) st;
  //printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, (DWORD) st, (DWORD) bRet);
  return bRet;
}

class SymInitializer
{
public:
  SymInitializer() : errorNum(0)
  {
    if (!::SymInitialize(::GetCurrentProcess(), NULL, TRUE))
      errorNum = GetLastError();
  }
  DWORD errorNum;
};

void *ABase::operator new(size_t size)
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  static SymInitializer loadTheSymbols;

  HANDLE currentProcess = ::GetCurrentProcess();

  AllocData *pdata = new  AllocData();
  pdata->ptr = (ABase *)::operator new(size);
  pdata->size = size;
  pdata->time = ATime::getHighPerfCounter();

  if (loadTheSymbols.errorNum)
  {
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
    std::cerr << "SymInitialize  GetLastError()=" << loadTheSymbols.errorNum << std::endl;
#endif
  }
  else
  {
    CONTEXT context;
    memset(&context, 0, sizeof(CONTEXT));
    context.ContextFlags = CONTEXT_ALL;
    ::RtlCaptureContext(&context);

    STACKFRAME64 stackframe;
    memset(&stackframe, 0, sizeof(STACKFRAME64));

    DWORD lastError = 0;
    for(int framenum = 0; ; ++framenum)
    {
      stackframe.AddrPC.Offset = context.Rip;
      stackframe.AddrPC.Mode = AddrModeFlat;
      stackframe.AddrFrame.Offset = context.Rsp;
      stackframe.AddrFrame.Mode = AddrModeFlat;
      stackframe.AddrStack.Offset = context.Rsp;
      stackframe.AddrStack.Mode = AddrModeFlat;

      if (!::StackWalk64(
        IMAGE_FILE_MACHINE_AMD64,
        currentProcess,
        ::GetCurrentThread(),
        &stackframe,
        &context,
        myReadProcMem,
        SymFunctionTableAccess64,
        SymGetModuleBase64,
        NULL
      ))
      {
        //a_Done
        break;
      }

      if (0 == stackframe.AddrPC.Offset)
      {
        lastError = GetLastError();
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
        std::cerr << "StackWalk64  GetLastError()=" << lastError << std::endl;
#endif
        break;
      }
      else if (0 == framenum)
      {
        //a_Skip over the current frame (this new operator)
        continue;
      }
      else
      {
        const int IMAGEHLP_MAX_NAMELEN = 1024;
        const int IMAGEHLP_BUFFER_SIZE = sizeof(IMAGEHLP_SYMBOL64)+IMAGEHLP_MAX_NAMELEN+1;
        u1 _internal_buffer[IMAGEHLP_BUFFER_SIZE];
        memset(_internal_buffer, 0, IMAGEHLP_BUFFER_SIZE);

        IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64 *)((void *)_internal_buffer);
        pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        pSymbol->MaxNameLength = IMAGEHLP_MAX_NAMELEN;

        DWORD64 offset = stackframe.AddrPC.Offset;
        if (!::SymGetSymFromAddr64(currentProcess, stackframe.AddrPC.Offset, &offset, pSymbol))
        {
          lastError = GetLastError();
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
          std::cerr << "SymGetSymFromAddr64  GetLastError()=" << lastError << std::endl;
#endif
          break;
        }
        else
        {
          StackData *pStackData = new StackData();
          if (!::UnDecorateSymbolName(pSymbol->Name, pStackData->undecorated, MAX_STACKDATA_BUFFER, UNDNAME_COMPLETE))
          {
            lastError = GetLastError();
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
            std::cerr << "UnDecorateSymbolName  GetLastError()=" << lastError << std::endl;
#endif
            delete pStackData;
            break;
          }
          else
          {
            DWORD ln = 0;
            IMAGEHLP_LINE64 line;
            memset(&line, 0, sizeof(IMAGEHLP_LINE64));
            if (!::SymGetLineFromAddr64(currentProcess, stackframe.AddrPC.Offset, &ln, &line))
            {
              lastError = GetLastError();
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
              std::cerr << "SymGetLineFromAddr64  GetLastError()=" << lastError << std::endl;
#endif
              delete pStackData;
              break;
            }
            else
            {
              // Got all the info needed
              strcpy_s(pStackData->filename, MAX_STACKDATA_FILE_BUFFER, line.FileName);
              pStackData->line = line.LineNumber;
              pdata->stackframes.push_back(pStackData);
            }
          }
        }
      }
    }
  }

  #ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
    std::cout << AllocDataWrapper(pdata) << std::endl;
  #endif

  AASSERT(NULL, GLOBAL_m_Allocations.end() == GLOBAL_m_Allocations.find(pdata->ptr));
  GLOBAL_m_Allocations[pdata->ptr] = pdata;
  return pdata->ptr;
}

void ABase::operator delete(void *p)
{
  if (p)
  {
    ALock lock(GLOBAL_mp_ConsoleSync);

    MAP_VOIDPTR_SIZET::iterator it = GLOBAL_m_Allocations.find(p);
    if (it != GLOBAL_m_Allocations.end())
    {
      //a_Check if we have an allocator to remove, may have been cleared so just ignore the fact it is missing
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
      std::cout << typeid(*(ABase *)p).name() << ":" << it->second << std::endl;
#endif
      GLOBAL_m_Allocations.erase(it);
    }
    else
    {
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
      std::cout << typeid(*(ABase *)p).name() << " not found in allocations during delete (warning)" << std::endl;
#endif
    }
    ::operator delete(p);
  }
}

void ABase::traceAllocations(AOutputBuffer& target, bool includeHexDump)
{
  ALock lock(GLOBAL_mp_ConsoleSync);

  size_t totalBytes = 0;
  target.append("count=",6);
  target.append(AString::fromSize_t(GLOBAL_m_Allocations.size()));
  target.append(AConstant::ASTRING_EOL);
  for (MAP_VOIDPTR_SIZET::iterator it = GLOBAL_m_Allocations.begin(); it != GLOBAL_m_Allocations.end(); ++it)
  {
    it->second->emit(target, includeHexDump);
    target.append(AConstant::ASTRING_EOL);
    totalBytes += it->second->size;
  }
  target.append("total_bytes=",12);
  target.append(AString::fromU8(totalBytes));
  target.append(AConstant::ASTRING_EOL);
}

void ABase::clearAllocations()
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  GLOBAL_m_Allocations.clear();
}

#endif
