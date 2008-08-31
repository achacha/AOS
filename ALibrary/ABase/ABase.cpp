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

#ifdef DEBUG_TRACK_ABASE_MEMORY
class AllocData
{
public:
  AllocData() : ptr(NULL), size(0), time(0) {}
  AllocData(const AllocData& that) : ptr(that.ptr), size(that.size), time(that.time) {}
  virtual ~AllocData() {}
  virtual void emit(AOutputBuffer& target, bool includeDebugDump) const
  {
    target.append(AString::fromU8(time));
    target.append(',');
    target.append(AString::fromPointer(ptr));
    target.append(',');
    target.append(AString::fromSize_t(size));
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
};

ASync_CriticalSection GLOBAL_mp_ConsoleSync;
typedef std::map<void *, AllocData> MAP_VOIDPTR_SIZET;
MAP_VOIDPTR_SIZET GLOBAL_m_Allocations;
#endif

ABase::ABase() :
  pPrev(NULL),
  pNext(NULL)
{
}

ABase::~ABase()
{
}

ABase *ABase::useNext()
{
  return pNext;
}

ABase *ABase::usePrev()
{
  return pPrev;
}

const ABase *ABase::getNext() const
{
  return pNext;
}

const ABase *ABase::getPrev() const
{
  return pPrev;
}

#ifdef DEBUG_TRACK_ABASE_MEMORY
void *ABase::operator new(size_t size)
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  AllocData data;
  data.ptr = (ABase *)::operator new(size);
  data.size = size;
  data.time = ATime::getHighPerfCounter();
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
  std::cout << data << std::endl;
#endif
  GLOBAL_m_Allocations[data.ptr]=data;
  return data.ptr;
}

void ABase::operator delete(void *p)
{
  if (p)
  {
    ALock lock(GLOBAL_mp_ConsoleSync);

    MAP_VOIDPTR_SIZET::iterator it = GLOBAL_m_Allocations.find(p);
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
    std::cout << it->second << std::endl;
#endif
    GLOBAL_m_Allocations.erase(it);

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
    it->second.emit(target, includeHexDump);
    target.append(AConstant::ASTRING_EOL);
    totalBytes += it->second.size;
  }
  target.append("total_bytes=",12);
  target.append(AString::fromU8(totalBytes));
  target.append(AConstant::ASTRING_EOL);
}
#endif
