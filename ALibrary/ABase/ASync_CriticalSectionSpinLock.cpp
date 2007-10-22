#include "pchABase.hpp"

#include "ASync_CriticalSectionSpinLock.hpp"
#include "ASystemException.hpp"

ASync_CriticalSectionSpinLock::ASync_CriticalSectionSpinLock(
  size_t spinCount,                    // = 4000
  ASynchronization::eInitialState i    // = UNLOCKED
) :
  mp_CriticalSection(new CRITICAL_SECTION),
  m_SpinCount(spinCount)
{
  if (!::InitializeCriticalSectionAndSpinCount(mp_CriticalSection, (DWORD)m_SpinCount))
    ATHROW_LAST_OS_ERROR(this);

  if (i==LOCKED) 
    lock();
}


ASync_CriticalSectionSpinLock::~ASync_CriticalSectionSpinLock()
{
  if (mp_CriticalSection)
    ::DeleteCriticalSection(mp_CriticalSection);
  delete mp_CriticalSection;
}

ASync_CriticalSectionSpinLock::ASync_CriticalSectionSpinLock(const ASync_CriticalSectionSpinLock& that) :
  m_SpinCount(AConstant::npos)
{
}

ASync_CriticalSectionSpinLock& ASync_CriticalSectionSpinLock::operator=(const ASync_CriticalSectionSpinLock&)
{
  return *this;
}

void ASync_CriticalSectionSpinLock::lock()
{
  ::EnterCriticalSection(mp_CriticalSection);
}


bool ASync_CriticalSectionSpinLock::trylock()
{
  return (::TryEnterCriticalSection(mp_CriticalSection) ? true : false);
}

void ASync_CriticalSectionSpinLock::unlock()
{
  if (mp_CriticalSection) 
    ::LeaveCriticalSection(mp_CriticalSection);
  else
    ATHROW(this, AException::InvalidObject);
}
