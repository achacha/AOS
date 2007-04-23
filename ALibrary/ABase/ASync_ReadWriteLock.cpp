#include "pchABase.hpp"

#include "ASync_ReadWriteLock.hpp"
#include "ASystemException.hpp"

ASync_ReadWriteLock::ASync_ReadWriteLock(
  const AString& strName, 
  u4 timeout,                         // = INFINITE
  ASynchronization::eInitialState i   // = UNLOCKED
):
  ASync_Mutex(strName, timeout, i)
{
}
    

ASync_ReadWriteLock::~ASync_ReadWriteLock()
{
}


void ASync_ReadWriteLock::lock()
{
  if (m_WriteLock.getLockCount() > 0)
    ASync_Mutex::lock();
}

bool ASync_ReadWriteLock::trylock()
{
  if (m_WriteLock.getLockCount() > 0)
    return ASync_Mutex::trylock();
  else
    return true;
}

void ASync_ReadWriteLock::unlock()
{
  if (m_WriteLock.getLockCount() > 0)
    return ASync_Mutex::unlock();
}
