#ifndef INCLUDED__ASync_ReadWriteLock_HPP__
#define INCLUDED__ASync_ReadWriteLock_HPP__

#include "apiABase.hpp"
#include "ASync_NOP.hpp"
#include "ASync_Mutex.hpp"

class ABASE_API ASync_ReadWriteLock : public ASync_Mutex
{
public:
	/*!
  strName - name of the read/write lock
  i - initial state
  */
  ASync_ReadWriteLock(
    const AString& strName, 
    u4 timeout = INFINITE,
    ASynchronization::eInitialState i = UNLOCKED
  );
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
  ASync_NOP m_WriteLock;

private:
	// disallow copy/assignment
	ASync_ReadWriteLock(const ASync_ReadWriteLock&);
	ASync_ReadWriteLock& operator=(const ASync_ReadWriteLock&);
};

#endif