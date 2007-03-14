#include "pchABase.hpp"

#include "AEvent.hpp"
#include "ASystemException.hpp"
#include "ADebugDumpable.hpp"

AEvent::AEvent(
  const AString& strName,
  u4 timeout,                        // = INFINITE
  bool autoReset,                    // = false
  ASynchronization::eInitialState i  // = UNLOCKED
):
  mstr_Name(strName),
  m_Timeout(timeout)
{
  m_handle = ::CreateEvent(NULL, autoReset, i == LOCKED, strName.c_str());
  if (!m_handle)
    ATHROW_LAST_OS_ERROR(this);
}
    

AEvent::~AEvent()
{
  try
  {
    if (m_handle)
      ::CloseHandle(m_handle);
  }
  catch(...)
  {
    //a_Prevent exception propagation in the dtor
  }
}


void AEvent::lock()
{
  if (m_handle)
  {
    ::ResetEvent(m_handle);
  	if (WAIT_FAILED == ::WaitForSingleObject(m_handle, m_Timeout))
      ATHROW_LAST_OS_ERROR(this);
  }
  else
  {
    ATHROW(this, AException::InvalidObject);
  }
}

bool AEvent::trylock()
{
  if (m_handle)
  {
    ::ResetEvent(m_handle);
    return (WAIT_TIMEOUT != ::WaitForSingleObject(m_handle, 0));
  }
  else
    ATHROW(this, AException::InvalidObject);
}

void AEvent::unlock()
{
	if (!::SetEvent(m_handle))
    ATHROW_LAST_OS_ERROR(this);
}

const AString& AEvent::getName()
{ 
  return mstr_Name; 
}

u4 AEvent::getTimeout() const
{
  return m_Timeout;
}

void AEvent::setTimeout(u4 timeout)
{
  m_Timeout = timeout;
}
