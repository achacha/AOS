#include "pchABase.hpp"

#include "ASync_ReadWriteLock.hpp"
#include "ASystemException.hpp"
#include "AThread.hpp"

ASync_ReadWriteLock::ASync_ReadWriteLock()
{
}
    

ASync_ReadWriteLock::~ASync_ReadWriteLock()
{
}


void ASync_ReadWriteLock::lock()
{
  std::cout.put('R');
  if (m_Locks.m_WriteState)
  {
    std::cout.put('B');
    m_Locks.m_ReadLock.lock();
  }
  m_Locks.m_ReadCounter.lock();
}

bool ASync_ReadWriteLock::trylock()
{
  if (m_Locks.m_WriteState)
    return m_Locks.m_ReadLock.trylock();

  m_Locks.m_ReadCounter.trylock();
  return true;
}

void ASync_ReadWriteLock::unlock()
{
  m_Locks.m_ReadCounter.unlock();
  if (m_Locks.m_WriteState)
  {
    std::cout.put('b');
    m_Locks.m_ReadLock.unlock();
  }
  std::cout.put('r');
  if (1 == m_Locks.m_WriteState && 0 == m_Locks.m_ReadCounter.getLockCount())
  {
    std::cout.put('e');
    m_Locks.m_ReadyToWrite.unlock();  //a_Signal that all readers are done and writing can start
  }
}

ASynchronization& ASync_ReadWriteLock::getWriteLock()
{
  return m_Locks;
}

ASync_ReadWriteLock::InternalReadWriteLock::InternalReadWriteLock()
{
}

ASync_ReadWriteLock::InternalReadWriteLock::~InternalReadWriteLock()
{
}

ASync_ReadWriteLock::InternalReadWriteLock::InternalReadWriteLock(const InternalReadWriteLock&)
{
}

ASync_ReadWriteLock::InternalReadWriteLock& ASync_ReadWriteLock::InternalReadWriteLock::operator=(const InternalReadWriteLock&)
{
  return *this;
}

void ASync_ReadWriteLock::InternalReadWriteLock::lock()
{
  std::cout.put('W');
  m_WriteLock.lock();   //a_One writer per lock
  
  ::InterlockedExchange(&m_WriteState, 1);  //a_Redirect readers to lock (waiting to write)

  m_ReadLock.lock();    //a_Block readers

  //a_Wait for all active readers to unlock
  if (m_ReadCounter.getLockCount() > 0)
  {
    std::cout.put('C');
    ALock lock(m_ReadyToWrite);
    std::cout.put('c');
  }
  ::InterlockedExchange(&m_WriteState, 2); //a_Writing
}

bool ASync_ReadWriteLock::InternalReadWriteLock::trylock()
{
  if (m_WriteLock.trylock())
  {
    ::InterlockedExchange(&m_WriteState, 1);
    m_ReadLock.lock();

    if (m_ReadCounter.getLockCount() > 0)
    {
      ALock lock(m_ReadyToWrite);
    }

    ::InterlockedExchange(&m_WriteState, 2);
    return true;
  }
  else
    return false;
}

void ASync_ReadWriteLock::InternalReadWriteLock::unlock()
{
  std::cout.put('w');
  m_ReadLock.unlock();                       //a_Unblok readers
  ::InterlockedExchange(&m_WriteState, 0);   //a_Redirect reader to not lock
  m_WriteLock.unlock();                      //a_Unlock writer
}
