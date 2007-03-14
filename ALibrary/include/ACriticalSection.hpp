#ifndef INCLUDED__ACriticalSection_HPP__
#define INCLUDED__ACriticalSection_HPP__

#include "apiABase.hpp"
#include "ASynchronization.hpp"

class ABASE_API ACriticalSection : public ASynchronization
{
public:
  ACriticalSection(ASynchronization::eInitialState i = UNLOCKED);
  virtual ~ACriticalSection();

	/*!
  ASynchronization
  */
  virtual void lock();
  virtual bool trylock();
  virtual void unlock();

private:
  //a_Prevent copy
  ACriticalSection(const ACriticalSection & that);
  ACriticalSection& operator=(const ACriticalSection & that);

  //a_Critical section object
#ifdef __WINDOWS__
  CRITICAL_SECTION *mp__CriticalSection; 
#endif
};

#endif
