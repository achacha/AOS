#include "pchABase.hpp"
#include "ASynch_NOP.hpp"

ASynch_NOP::ASynch_NOP(
  eInitialState i //= ASynchronization::UNLOCKED
)
{
  if (ASynchronization::LOCKED == i)
    ++m_LockCount;
}

ASynch_NOP::~ASynch_NOP()
{
}

void ASynch_NOP::lock()
{
  ++m_LockCount;
}

bool ASynch_NOP::trylock()
{
  return true;  //a_Can always lock
}

void ASynch_NOP::unlock()
{
  --m_LockCount;
}

int ASynch_NOP::getLockCount() const
{
  return m_LockCount;
}
