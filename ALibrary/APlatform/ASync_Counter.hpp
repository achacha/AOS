/*
Written by Alex Chachanashvili

$Id: ASync_Counter.hpp 205 2008-05-29 13:58:16Z achacha $
*/
#ifndef INCLUDED_ASync_Counter_HPP_
#define INCLUDED_ASync_Counter_HPP_

#include "apiAPlatform.hpp"
#include "ASynchronization.hpp"

/*!
NOP ASynchronization class

Does nothing when called for lock and unlock, just keeps track of the calls
*/
class APLATFORM_API ASync_Counter : public ASynchronization
{
public:
  ASync_Counter();
  virtual ~ASync_Counter();

public:
  virtual void lock();       //a_Lock this object
  virtual bool trylock();    //a_Try and lock, return status
  virtual void unlock();     //a_Unlock this object

  /*!
  Access to counter
  */
  long getLockCount() const;

  /*!
  Manipulation of counter
  */
  void reset(long value = 0);

private:
  volatile long m_LockCount;
};

#endif //INCLUDED_ASynch_NOP_HPP_

