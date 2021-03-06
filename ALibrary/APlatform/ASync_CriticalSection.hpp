/*
Written by Alex Chachanashvili

$Id: ASync_CriticalSection.hpp 205 2008-05-29 13:58:16Z achacha $
*/
#ifndef INCLUDED__ASync_CriticalSection_HPP__
#define INCLUDED__ASync_CriticalSection_HPP__

#include "apiAPlatform.hpp"
#include "ASynchronization.hpp"

class APLATFORM_API ASync_CriticalSection : public ASynchronization
{
public:
  ASync_CriticalSection(ASynchronization::eInitialState i = UNLOCKED);
  virtual ~ASync_CriticalSection();

	/*!
  ASynchronization
  */
  virtual void lock();
  virtual bool trylock();
  virtual void unlock();

private:
  //a_Prevent copy
  ASync_CriticalSection(const ASync_CriticalSection & that);
  ASync_CriticalSection& operator=(const ASync_CriticalSection & that);

  //a_Critical section object
#ifdef __WINDOWS__
  CRITICAL_SECTION *mp__CriticalSection; 
#endif
};

#endif
