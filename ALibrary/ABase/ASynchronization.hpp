/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ASynchronization_HPP__
#define INCLUDED__ASynchronization_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ALock.hpp"

/*!
Synchronization class

-------------BEGIN HPP CUT--------------------
public:
  virtual void lock();
  virtual bool trylock();
  virtual void unlock();
-------------END HPP CUT----------------------
*/
class ABASE_API ASynchronization : public ADebugDumpable
{
public:
  enum eInitialState
  {
    UNLOCKED = 0x0,
    LOCKED   = 0x1
  };

  virtual ~ASynchronization();

  virtual void lock()    = 0;       //a_Lock this object
  virtual bool trylock() = 0;       //a_Try and lock, return status
  virtual void unlock()  = 0;       //a_Unlock this object
};

#endif

