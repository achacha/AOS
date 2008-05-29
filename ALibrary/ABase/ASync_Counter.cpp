/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ASync_Counter.hpp"

ASync_Counter::ASync_Counter() :
  m_LockCount(0)
{
}

ASync_Counter::~ASync_Counter()
{
}

void ASync_Counter::lock()
{
  ::InterlockedIncrement(&m_LockCount);
}

bool ASync_Counter::trylock()
{
  ::InterlockedIncrement(&m_LockCount);
  return true;  //a_Can always lock
}

void ASync_Counter::unlock()
{
  ::InterlockedDecrement(&m_LockCount);
}

long ASync_Counter::getLockCount() const
{
  return m_LockCount;
}

void ASync_Counter::reset(long value)
{
  ::InterlockedExchange(&m_LockCount, value);
}
