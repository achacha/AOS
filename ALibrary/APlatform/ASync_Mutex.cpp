/*
Written by Alex Chachanashvili

$Id: ASync_Mutex.cpp 205 2008-05-29 13:58:16Z achacha $
*/
#include "pchAPlatform.hpp"
#include "ASync_Mutex.hpp"
#include "ASystemException.hpp"
#include "ATextGenerator.hpp"

ASync_Mutex::ASync_Mutex(
  u4 timeout,                         // = INFINITE
  ASynchronization::eInitialState i   // = UNLOCKED
):
  m_Timeout(timeout)
{
  ATextGenerator::generateUniqueId(mstr_Name);
  m_handle = ::CreateMutex(NULL, i == LOCKED, mstr_Name.c_str());
  if (!m_handle)
    ATHROW_LAST_OS_ERROR(this);
}

ASync_Mutex::ASync_Mutex(
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
    

ASync_Mutex::~ASync_Mutex()
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


void ASync_Mutex::lock()
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

bool ASync_Mutex::trylock()
{
  if (m_handle)
    return (WAIT_TIMEOUT != ::WaitForSingleObject(m_handle, 0));
  else
    ATHROW(this, AException::InvalidObject);
}

void ASync_Mutex::unlock()
{
	if (!::ReleaseMutex(m_handle))
    ATHROW_LAST_OS_ERROR(this);
}

const AString& ASync_Mutex::getName()
{ 
  return mstr_Name; 
}

u4 ASync_Mutex::getTimeout() const
{
  return m_Timeout;
}

void ASync_Mutex::setTimeout(u4 timeout)
{
  m_Timeout = timeout;
}
