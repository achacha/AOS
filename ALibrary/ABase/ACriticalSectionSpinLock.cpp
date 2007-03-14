#include "pchABase.hpp"

#include "ACriticalSectionSpinLock.hpp"
#include "AException.hpp"

ACriticalSectionSpinLock::ACriticalSectionSpinLock(
  u4 spinCount,                        // = 4000
  ASynchronization::eInitialState i    // = UNLOCKED
) :
  mp_CriticalSection(new CRITICAL_SECTION),
  m_SpinCount(spinCount)
{
  ::InitializeCriticalSectionAndSpinCount(mp_CriticalSection, m_SpinCount);

  if (i==LOCKED) 
    lock();
}


ACriticalSectionSpinLock::~ACriticalSectionSpinLock()
{
  if (mp_CriticalSection)
    ::DeleteCriticalSection(mp_CriticalSection);
  delete mp_CriticalSection;
}

ACriticalSectionSpinLock::ACriticalSectionSpinLock(const ACriticalSectionSpinLock& that) :
  m_SpinCount(AConstant::npos)
{
}

ACriticalSectionSpinLock& ACriticalSectionSpinLock::operator=(const ACriticalSectionSpinLock&)
{
  return *this;
}

void ACriticalSectionSpinLock::lock()
{
  ::EnterCriticalSection(mp_CriticalSection);
}


bool ACriticalSectionSpinLock::trylock()
{
  return (::TryEnterCriticalSection(mp_CriticalSection) ? true : false);
}

void ACriticalSectionSpinLock::unlock()
{
  if (mp_CriticalSection) 
    ::LeaveCriticalSection(mp_CriticalSection);
  else
    ATHROW(this, AException::InvalidObject);
}
