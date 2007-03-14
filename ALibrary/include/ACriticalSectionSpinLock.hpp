#ifndef INCLUDED__ACriticalSectionSpinLock_HPP__
#define INCLUDED__ACriticalSectionSpinLock_HPP__

#include "apiABase.hpp"
#include "ASynchronization.hpp"

class ABASE_API ACriticalSectionSpinLock : public ASynchronization
{
public:
  /*!
  A synchronization optimized for speed to avoid sleep mode and attempt to lock with busy looping

  spinCount - how many times to loop trying to get a lock
    if lock is not acquired during the lock goes into sleep mode
    
    "You can improve performance significantly by choosing a small 
     spin count for a critical section of short duration. The heap 
     manager uses a spin count of roughly 4000 for its per-heap 
     critical sections. This gives great performance and scalability 
     in almost all worst-case scenarios." - MSDN help
  */
  ACriticalSectionSpinLock(u4 spinCount = 4000, ASynchronization::eInitialState i = UNLOCKED);
  virtual ~ACriticalSectionSpinLock();

	/*!
  ASynchronization
  */
  virtual void lock();
  virtual bool trylock();
  virtual void unlock();

private:
  //a_Prevent copy
  ACriticalSectionSpinLock(const ACriticalSectionSpinLock & that);
  ACriticalSectionSpinLock& operator=(const ACriticalSectionSpinLock & that);

  //a_Critical section object
#ifdef __WINDOWS__
  CRITICAL_SECTION *mp_CriticalSection;
  u4 m_SpinCount;
#endif
};

#endif
