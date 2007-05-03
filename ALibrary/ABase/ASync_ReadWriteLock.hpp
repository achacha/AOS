#ifndef INCLUDED__ASync_ReadWriteLock_HPP__
#define INCLUDED__ASync_ReadWriteLock_HPP__

#include "apiABase.hpp"
#include "ASync_Counter.hpp"
#include "ASync_Mutex.hpp"
#include "ASync_CriticalSection.hpp"
#include "ASync_Event.hpp"

/*!
Read-Write lock, unlimited read until write locked, then read is blocked until write lock is unlocked

Example:

ASync_ReadWriteLock myLock;

...

void read()
{
  ALock lock(myLock);     // reader
  ...
}

void write()
{
  ALock lock(myLock.getWriteLock());

  // while here no read or other write is allowed
}

*/
class ABASE_API ASync_ReadWriteLock : public ASynchronization
{
public:
	/*!
  pReadLock is sync object that is owned and deleted by this object, it is used as read lock when write lock is locked
  */
  ASync_ReadWriteLock();
	virtual ~ASync_ReadWriteLock();

	/*!
  ASynchronization
  */
  virtual void lock();
	virtual bool trylock();
	virtual void unlock();

  /*!
  Access to the write object, when locked read locks will be blocked

  ALock lock(myrwl.getWriteLock());  // This is the preferred way to write lock
  */
  ASynchronization& getWriteLock();

protected:
  class InternalReadWriteLock : public ASynchronization
  {
  public:
    InternalReadWriteLock();
    virtual ~InternalReadWriteLock();

	  /*!
    ASynchronization
    */
    virtual void lock();
	  virtual bool trylock();
	  virtual void unlock();

    ASync_CriticalSection m_ReadLock;       //a_Locked when no more readers
    ASync_Counter m_ReadCounter;            //a_Counts # of readers active
    ASync_CriticalSection m_WriteLock;      //a_Only writer per lock
    ASync_Event m_ReadyToWrite;             //a_Signaled when no more readers
    
    /*
    0 - clear
    1 - Writer waiting to write, if set then on 0 readers event is triggered
    2 - Writing
    */
    volatile long m_WriteState;

  private:
	  // disallow copy/assignment
	  InternalReadWriteLock(const InternalReadWriteLock&);
	  InternalReadWriteLock& operator=(const InternalReadWriteLock&);
  } m_Locks;

private:
	// disallow copy/assignment
	ASync_ReadWriteLock(const ASync_ReadWriteLock&);
	ASync_ReadWriteLock& operator=(const ASync_ReadWriteLock&);
};

#endif