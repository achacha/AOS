/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ASync_CriticalSection.hpp"
#include "AException.hpp"

ASync_CriticalSection::ASync_CriticalSection(ASynchronization::eInitialState i) :
  mp__CriticalSection(new CRITICAL_SECTION)
{
  ::InitializeCriticalSection(mp__CriticalSection);

  if (i==LOCKED) 
    lock();
}


ASync_CriticalSection::~ASync_CriticalSection()
{
  if (mp__CriticalSection)
    ::DeleteCriticalSection(mp__CriticalSection);
  delete mp__CriticalSection;
}

ASync_CriticalSection::ASync_CriticalSection(const ASync_CriticalSection&)
{
}

ASync_CriticalSection& ASync_CriticalSection::operator=(const ASync_CriticalSection&)
{
  return *this;
}

void ASync_CriticalSection::lock()
{
  ::EnterCriticalSection(mp__CriticalSection);
}


bool ASync_CriticalSection::trylock()
{
  return (::TryEnterCriticalSection(mp__CriticalSection) ? true : false);
}

void ASync_CriticalSection::unlock()
{
  if (mp__CriticalSection) 
    ::LeaveCriticalSection(mp__CriticalSection);
  else
    ATHROW(this, AException::InvalidObject);
}
