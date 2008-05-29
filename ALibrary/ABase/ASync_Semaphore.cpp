/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ASync_Semaphore.hpp"
#include "ASystemException.hpp"

ASync_Semaphore::ASync_Semaphore(
  const AString& strName, 
  size_t maxCount,                    // = 1
  ASynchronization::eInitialState i   // = UNLOCKED
) :
  m_maxCount(maxCount),
  m_handle(NULL),
  mstr_Name(strName)
{  
  //a_First try to open a semaphore
  m_handle = ::OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, strName.c_str());
  
  //a_Create if it doesn't exist
  if (!m_handle)
    m_handle = ::CreateSemaphore(NULL, (i == UNLOCKED) ? maxCount : maxCount-1, maxCount, strName.c_str());

  if (!m_handle)
    ATHROW_LAST_OS_ERROR(this);
}

ASync_Semaphore::~ASync_Semaphore()
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

void ASync_Semaphore::lock()
{
  if (m_handle)
  {
    if (WAIT_FAILED == ::WaitForSingleObjectEx( m_handle, INFINITE, TRUE ))
      ATHROW_LAST_OS_ERROR(this);
  }
  else
  {
    ATHROW(this, AException::InvalidObject);
  }
}

bool ASync_Semaphore::trylock()
{
  if (m_handle)
    return (WAIT_TIMEOUT != ::WaitForSingleObjectEx(m_handle, 0, TRUE));
  else
    ATHROW(this, AException::InvalidObject);
}

void ASync_Semaphore::unlock()
{
  if (!::ReleaseSemaphore( m_handle, 1, NULL ))
    ATHROW_LAST_OS_ERROR(this);
}

