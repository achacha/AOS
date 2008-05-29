/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ALock.hpp"
#include "ASynchronization.hpp"

ALock::ALock() : 
  me__LockMode(ALock::UNLOCKED),
  mp__Synchronization(NULL)
{ 
  lock();
}
 
ALock::ALock(ASynchronization& synchronization, eLockMode mode) :
  me__LockMode(ALock::UNLOCKED),
  mp__Synchronization(&synchronization) 
{ 
  if (mode == ALock::LOCKED) 
    lock();
}

ALock::ALock(ASynchronization* psynchronization, eLockMode mode) :
  me__LockMode(ALock::UNLOCKED),
  mp__Synchronization(psynchronization) 
{ 
  if (mode == ALock::LOCKED) 
    lock();
}

ALock::ALock(const ALock& that) :
  me__LockMode(ALock::UNLOCKED),
  mp__Synchronization(that.mp__Synchronization) 
{
  if (that.me__LockMode == ALock::LOCKED) 
    lock();
}

ALock& ALock::operator=(const ALock& that) 
{
  unlock(); 

  mp__Synchronization = that.mp__Synchronization;
  if (that.me__LockMode == ALock::LOCKED) 
    lock();

  return *this;
}

ALock::~ALock()
{
  unlock();
}

void ALock::lock()
{
  if (!mp__Synchronization) 
    return;

  AASSERT(NULL, me__LockMode == ALock::UNLOCKED);
  if (me__LockMode == ALock::UNLOCKED) 
    mp__Synchronization->lock();
  
  me__LockMode = ALock::LOCKED;
}

bool ALock::trylock() 
{
  if (!mp__Synchronization) 
    return ALock::UNLOCKED;

  AASSERT(NULL, me__LockMode == ALock::UNLOCKED);
  if (me__LockMode == ALock::LOCKED) 
    return false;

  bool boolTry = mp__Synchronization->trylock();
  me__LockMode = (boolTry ? ALock::LOCKED : ALock::UNLOCKED);

  return boolTry;
}

void ALock::unlock()
{
  if (!mp__Synchronization) 
    return;

  if (me__LockMode == ALock::LOCKED) 
    mp__Synchronization->unlock(); 

  me__LockMode = ALock::UNLOCKED;
}
