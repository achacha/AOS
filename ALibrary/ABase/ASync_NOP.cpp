#include "pchABase.hpp"
#include "ASync_NOP.hpp"

ASync_NOP::ASync_NOP(
  eInitialState i //= ASynchronization::UNLOCKED
)
{
  if (ASynchronization::LOCKED == i)
    ++m_LockCount;
}

ASync_NOP::~ASync_NOP()
{
}

void ASync_NOP::lock()
{
  ++m_LockCount;
}

bool ASync_NOP::trylock()
{
  return true;  //a_Can always lock
}

void ASync_NOP::unlock()
{
  --m_LockCount;
}

int ASync_NOP::getLockCount() const
{
  return m_LockCount;
}
