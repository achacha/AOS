#include "pchABase.hpp"

#include "ACriticalSection.hpp"
#include "AException.hpp"

ACriticalSection::ACriticalSection(ASynchronization::eInitialState i) :
  mp__CriticalSection(new CRITICAL_SECTION)
{
  ::InitializeCriticalSection(mp__CriticalSection);

  if (i==LOCKED) 
    lock();
}


ACriticalSection::~ACriticalSection()
{
  if (mp__CriticalSection)
    ::DeleteCriticalSection(mp__CriticalSection);
  delete mp__CriticalSection;
}

ACriticalSection::ACriticalSection(const ACriticalSection&)
{
}

ACriticalSection& ACriticalSection::operator=(const ACriticalSection&)
{
  return *this;
}

void ACriticalSection::lock()
{
  ::EnterCriticalSection(mp__CriticalSection);
}


bool ACriticalSection::trylock()
{
  return (::TryEnterCriticalSection(mp__CriticalSection) ? true : false);
}

void ACriticalSection::unlock()
{
  if (mp__CriticalSection) 
    ::LeaveCriticalSection(mp__CriticalSection);
  else
    ATHROW(this, AException::InvalidObject);
}
