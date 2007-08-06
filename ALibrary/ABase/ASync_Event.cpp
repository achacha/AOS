#include "pchABase.hpp"

#include "ASync_Event.hpp"
#include "ASystemException.hpp"
#include "ADebugDumpable.hpp"
#include "ATextGenerator.hpp"

ASync_Event::ASync_Event(
  u4 timeout,                        // = INFINITE
  bool autoReset,                    // = false
  ASynchronization::eInitialState i  // = UNLOCKED
):
  m_Timeout(timeout)
{
  ATextGenerator::generateUniqueId(mstr_Name, 32);
  m_handle = ::CreateEvent(NULL, autoReset, i == LOCKED, mstr_Name.c_str());
  if (!m_handle)
    ATHROW_LAST_OS_ERROR(this);
}

ASync_Event::ASync_Event(
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


ASync_Event::~ASync_Event()
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


void ASync_Event::lock()
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

bool ASync_Event::trylock()
{
  if (m_handle)
  {
    ::ResetEvent(m_handle);
    return (WAIT_TIMEOUT != ::WaitForSingleObject(m_handle, 0));
  }
  else
    ATHROW(this, AException::InvalidObject);
}

void ASync_Event::unlock()
{
	if (!::SetEvent(m_handle))
    ATHROW_LAST_OS_ERROR(this);
}

const AString& ASync_Event::getName()
{ 
  return mstr_Name; 
}

u4 ASync_Event::getTimeout() const
{
  return m_Timeout;
}

void ASync_Event::setTimeout(u4 timeout)
{
  m_Timeout = timeout;
}
