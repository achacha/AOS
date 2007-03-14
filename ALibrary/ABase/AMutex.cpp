#include "pchABase.hpp"

#include "AMutex.hpp"
#include "ASystemException.hpp"
#include "ADebugDumpable.hpp"

AMutex::AMutex(
  const AString& strName, 
  u4 timeout,                         // = INFINITE
  ASynchronization::eInitialState i   // = UNLOCKED
):
  mstr_Name(strName),
  m_Timeout(timeout)
{
  m_handle = ::CreateMutex(NULL, i == LOCKED, strName.c_str());
  if (!m_handle)
    ATHROW_LAST_OS_ERROR(this);
}
    

AMutex::~AMutex()
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


void AMutex::lock()
{
  if (m_handle)
  {
  	if (WAIT_FAILED == ::WaitForSingleObject(m_handle, m_Timeout))
      ATHROW_LAST_OS_ERROR(this);
  }
  else
  {
    ATHROW(this, AException::InvalidObject);
  }
}

bool AMutex::trylock()
{
  if (m_handle)
    return (WAIT_TIMEOUT != ::WaitForSingleObject(m_handle, 0));
  else
    ATHROW(this, AException::InvalidObject);
}

void AMutex::unlock()
{
	if (!::ReleaseMutex(m_handle))
    ATHROW_LAST_OS_ERROR(this);
}

const AString& AMutex::getName()
{ 
  return mstr_Name; 
}

u4 AMutex::getTimeout() const
{
  return m_Timeout;
}

void AMutex::setTimeout(u4 timeout)
{
  m_Timeout = timeout;
}
