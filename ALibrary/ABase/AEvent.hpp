#ifndef INCLUDED__AEvent_HPP__
#define INCLUDED__AEvent_HPP__

#include "apiABase.hpp"
#include "ASynchronization.hpp"
#include "AString.hpp"

class ABASE_API AEvent : public ASynchronization
{
public:

#if defined(__WINDOWS__)
	typedef HANDLE Handle;
#endif

public:
	/*!
  strEventName - unique name of the event
  timeout - in milliseconds, it will wait for event or timeout (INFINITE means it will wait for event signal only)
  autoReset - if true the event will reset right after being signaled
  i - initial state (UNLOCKED by default)
  */
  AEvent(
    const AString& strEventName, 
    u4 timeout = INFINITE,
    bool autoReset = false, 
    ASynchronization::eInitialState i = UNLOCKED
  );
	virtual ~AEvent();

	/*!
  ASynchronization
  */
  virtual void lock();          //a_Wait for event or timeout
	virtual bool trylock();       //a_Try to wait for an event
	virtual void unlock();        //a_Signal event (if it hasn't timed out yet)

  /*!
  Event name
  */
  const AString& getName();

  /*!
  Event timeout in milliseconds
  */
  u4 getTimeout() const;
  void setTimeout(u4 timeout = INFINITE);

private:
	// disallow copy/assignment
	AEvent(const AEvent&);
	AEvent& operator=(const AEvent&);

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
