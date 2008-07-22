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
  std::cout << AString::fromPointer(p) << "\tnew(" << size << "), typeid(" << typeid(p).name() << ")" << std::endl;
  return p;
}

void ABase::operator delete(void *p)
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  std::cout << AString::fromPointer(p) << "\tdelete" << std::endl;
  ::operator delete(p);
}

void *ABase::operator new[](size_t size)
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  void *p = ::operator new(size);
  std::cout << AString::fromPointer(p) << "\tnew[" << size << "]" << std::endl;
  return p;
}

void ABase::operator delete[](void *p)
{
  ALock lock(GLOBAL_mp_ConsoleSync);
  std::cout << AString::fromPointer(p) << "\tdelete[]"  << std::endl;
  ::operator delete(p);
}
#endif
