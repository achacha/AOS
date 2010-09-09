/*
Written by Alex Chachanashvili

$Id: ASync_Semaphore.cpp 301 2009-09-14 22:36:18Z achacha $
*/
#include "pchAPlatform.hpp"
#include "ASync_Semaphore.hpp"
#include "ASystemException.hpp"

#if defined(__LINUX__)
#include <fcntl.h>
#endif

ASync_Semaphore::ASync_Semaphore(
  const AString& strName,
  size_t maxCount,                    // = 1
  ASynchronization::eInitialState i   // = UNLOCKED
) :
  m_maxCount(maxCount),
  m_handle(NULL),
  mstr_Name(strName)
{
#if defined(__WINDOWS__)
  //a_First try to open a semaphore
  m_handle = ::OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, strName.c_str());

  //a_Create if it doesn't exist
  if (!m_handle)
    m_handle = ::CreateSemaphore(NULL, (i == UNLOCKED) ? maxCount : maxCount-1, maxCount, strName.c_str());

  if (!m_handle)
    ATHROW_LAST_OS_ERROR(this);
#elif defined(__LINUX__)
  //a_Open or create
  m_handle = sem_open(strName.c_str(), O_CREAT);

  //a_On failure throw
  if (!m_handle)
  {
      ATHROW_LAST_OS_ERROR(this);
  }
  else
    sem_init(m_handle, 0, maxCount);
#endif
}

ASync_Semaphore::~ASync_Semaphore()
{
#if defined(__WINDOWS__)
  try
  {
    if (m_handle)
      ::CloseHandle(m_handle);
  }
  catch(...)
  {
    //a_Prevent exception propagation in the dtor
  }
#elif defined(__LINUX__)
  try
  {
    if (m_handle)
      sem_close(m_handle);
      sem_unlink(mstr_Name.c_str());
  }
  catch(...)
  {
    //a_Prevent exception propagation in the dtor
  }
#endif
}

void ASync_Semaphore::lock()
{
#if defined(__WINDOWS__)
  if (m_handle)
  {
    if (WAIT_FAILED == ::WaitForSingleObjectEx( m_handle, INFINITE, TRUE ))
      ATHROW_LAST_OS_ERROR(this);
  }
  else
  {
    ATHROW(this, AException::InvalidObject);
  }
#elif defined(__LINUX__)
  if (m_handle)
  {
    if (sem_wait(m_handle))
      ATHROW_LAST_OS_ERROR(this);
  }
  else
  {
    ATHROW(this, AException::InvalidObject);
  }
#endif
}

bool ASync_Semaphore::trylock()
{
#if defined(__WINDOWS__)
  if (m_handle)
    return (WAIT_TIMEOUT != ::WaitForSingleObjectEx(m_handle, 0, TRUE));
  else
    ATHROW(this, AException::InvalidObject);
#elif defined(__LINUX__)
  if (m_handle)
    return (0 == sem_trywait(m_handle));
  else
    ATHROW(this, AException::InvalidObject);
#endif
}

void ASync_Semaphore::unlock()
{
#if defined(__WINDOWS__)
  if (m_handle)
  {
    if (!::ReleaseSemaphore( m_handle, 1, NULL ))
      ATHROW_LAST_OS_ERROR(this);
  }
  else
    ATHROW(this, AException::InvalidObject);
#elif defined(__LINUX__)
  if (m_handle)
  {
    if (sem_post(m_handle))
      ATHROW_LAST_OS_ERROR(this);
  }
  else
    ATHROW(this, AException::InvalidObject);
#endif
}

