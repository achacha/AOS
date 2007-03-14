#ifndef INCLUDED__AMutex_HPP__
#define INCLUDED__AMutex_HPP__

#include "apiABase.hpp"
#include "ASynchronization.hpp"
#include "AString.hpp"

// AMutex
//
// This provides a native mutex that does not guarantee recursion
//

class ABASE_API AMutex : public ASynchronization
{
public:

#if defined(__WINDOWS__)
	typedef HANDLE Handle;
#endif

public:
	/*!
  strMutexName - unique name for this mutex
  timeout - milliseconds until it ulocks itself, if INFINITE it will wait until signaled
  i - initial state
  */
  AMutex(
    const AString& strMutexName, 
    u4 timeout = INFINITE,
    ASynchronization::eInitialState i = UNLOCKED
  );
	virtual ~AMutex();

	/*!
  ASynchronization
  */
  virtual void lock();
	virtual bool trylock();
	virtual void unlock();

  /*!
  Mutex name
  */
  const AString& getName();

  /*!
  Mutex timeout in milliseconds
  */
  u4 getTimeout() const;
  void setTimeout(u4 timeout = INFINITE);

private:
	// disallow copy/assignment
	AMutex(const AMutex&);
	AMutex& operator=(const AMutex&);

  //a_Name of this mutex (used for locking)
  AString mstr_Name;

  //a_Timeout
  u4 m_Timeout;

	// Implementation members are platform specific!
#if defined(__WINDOWS__)
protected:
	HANDLE m_handle;
#endif
};

#endif /* __Mutex_HPP */
