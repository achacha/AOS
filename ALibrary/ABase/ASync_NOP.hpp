#ifndef INCLUDED_ASync_NOP_HPP_
#define INCLUDED_ASync_NOP_HPP_

#include "apiABase.hpp"
#include "ASynchronization.hpp"

/*!
NOP ASynchronization class

Does nothing when called for lock and unlock, just keeps track of the calls
*/
class ABASE_API ASync_NOP : public ASynchronization
{
public:
  ASync_NOP(eInitialState i = ASynchronization::UNLOCKED);
  virtual ~ASync_NOP();

public:
  virtual void lock();       //a_Lock this object
  virtual bool trylock();    //a_Try and lock, return status
  virtual void unlock();     //a_Unlock this object

  int getLockCount() const;

private:
  int m_LockCount;
};

#endif //INCLUDED_ASynch_NOP_HPP_

