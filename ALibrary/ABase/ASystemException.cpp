//
// ASystemException
//

#include "pchABase.hpp"

#include "ASystemException.hpp"

ASystemException::ASystemException
(
  const ADebugDumpable *const pObject,
  int iID,
  const char* pccFilename,
  int iLineNumber,
  const AString& strExtra
) :
  AException(pObject, iID, pccFilename, iLineNumber, strExtra),
  m_lastOSError(0)
{
}

ASystemException::ASystemException
(
  u4 lastOSError,
  const ADebugDumpable *const pObject,
  const char* pccFilename,
  int iLineNumber,
  const AString& strExtra
) :
  AException(pObject, AException::SystemError, pccFilename, iLineNumber, strExtra),
  m_lastOSError(lastOSError)
{
}

ASystemException::ASystemException(const ASystemException &that) :
  AException(that)
{
  m_lastOSError = that.m_lastOSError;
}

void ASystemException::getDescription(AOutputBuffer& target) const throw()
{
  switch(_getID())
  {
    case SystemError:
      if (m_lastOSError > 0)
        __getLastOSError(target, m_lastOSError);
      else
        __getLastOSError(target, ::GetLastError());
    break;

    default:
      AException::getDescription(target);
  }
}

void ASystemException::setLastOSError(u4 lastOSError)
{
  m_lastOSError = lastOSError;
}

#ifdef __WINDOWS__
void ASystemException::__getLastOSError(AOutputBuffer& target, u4 lastOSError) const throw()
{
  LPVOID lpMsgBuf;
  FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM | 
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      lastOSError,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0,
      NULL 
  );
  target.append(ASWNL((LPCSTR)lpMsgBuf));
  LocalFree(lpMsgBuf);
}
#else
void ASystemException::__getLastOSError(AOutputBuffer& target, u4 lastOSError) const throw()
{
  target.append(ASW("errno=",6));
  target.append(AString::fromLong(errno));
}
#endif
