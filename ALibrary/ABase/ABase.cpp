/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ABase.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AString.hpp"

#ifdef DEBUG_TRACK_ABASE_MEMORY
ASync_CriticalSection GLOBAL_mp_ConsoleSync;
typedef std::map<void *, size_t> MAP_VOIDPTR_SIZET;
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
  void *p = ::operator new(size);
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
  std::cout << AString::fromPointer(p) << "\tnew(" << size << "), typeid(" << typeid(p).name() << ")" << std::endl;
#endif
  GLOBAL_m_Allocations[p]=size;
  return p;
}

void ABase::operator delete(void *p)
{
  if (p)
  {
    ALock lock(GLOBAL_mp_ConsoleSync);

    MAP_VOIDPTR_SIZET::iterator it = GLOBAL_m_Allocations.find(p);
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
    std::cout << AString::fromPointer(p) << "\tdelete size=" << it->second << std::endl;
#endif
    GLOBAL_m_Allocations.erase(it);

    ::operator delete(p);
  }
}

void *ABase::operator new[](size_t size)
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  void *p = ::operator new(size);
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
  std::cout << AString::fromPointer(p) << "\tnew[" << size << "]" << std::endl;
#endif
  GLOBAL_m_Allocations[p]=size;
  return p;
}

void ABase::operator delete[](void *p)
{
  if (p)
  {
    ALock lock(GLOBAL_mp_ConsoleSync);
    MAP_VOIDPTR_SIZET::iterator it = GLOBAL_m_Allocations.find(p);
#ifdef DEBUG_TRACK_ABASE_MEMORY_DISPLAY_TO_COUT
    std::cout << AString::fromPointer(p) << "\tdelete[] size=" << it->second << std::endl;
#endif
    GLOBAL_m_Allocations.erase(it);

    ::operator delete(p);
  }
}

void ABase::traceAllocations(AOutputBuffer& target, bool includeHexDump)
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  target.append("count=",6);
  target.append(AString::fromSize_t(GLOBAL_m_Allocations.size()));
  target.append(AConstant::ASTRING_EOL);
  for (MAP_VOIDPTR_SIZET::iterator it = GLOBAL_m_Allocations.begin(); it != GLOBAL_m_Allocations.end(); ++it)
  {
    target.append(AString::fromPointer(it->first));
    target.append(',');
    target.append(AString::fromSize_t(it->second));
    target.append(AConstant::ASTRING_EOL);
    if (includeHexDump)
    {
      ADebugDumpable::dumpMemory_HexAscii(target, it->first, it->second, 2, 128);
      target.append(AConstant::ASTRING_EOL);
    }
  }
}
#endif
