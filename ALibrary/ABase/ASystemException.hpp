/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__ASystemException_HPP__
#define INCLUDED__ASystemException_HPP__

#include "apiABase.hpp"
#include "AException.hpp"

class ABASE_API ASystemException : public AException
{
public:
	/*!
  Default construction is of an unknown exception
  */
  ASystemException(
    const ADebugDumpable *const pObject = NULL,
    int iID = AException::Unknown, 
    const char* pccFilename = "", 
    int iLineNumber = 0x0, 
    const AString& strExtra = AConstant::ASTRING_EMPTY
  );
	
	/*!
  Construct exception with LastOSErrorAlreadyKnown already known from a call to GetLastError or similar
  */
  ASystemException(
    u4 lastOSError, 
    const ADebugDumpable *const pObject = NULL,
    const char* pccFilename = "", 
    int iLineNumber = 0x0, 
    const AString& strExtra = AConstant::ASTRING_EMPTY
  );

  /*!
  Copy ctor
  */
  ASystemException(const ASystemException& that);

  /*!
  Special case when we have the last system error and just need to set it when OSErrorToBeSet is used
  */
  void setLastOSError(u4);

  /*!
  AException
  */
  virtual void getDescription(AOutputBuffer& target) const throw();

private:
  void __getLastOSError(AOutputBuffer& target, u4 error) const throw();
  u4 m_lastOSError;
};

/*!
Throws a system exception based on last error code
If we already know the error # (if we called GetLastError or similar, 2nd macro is to be used
*/
#define ATHROW_LAST_OS_ERROR(debugdumpable) throw ASystemException(debugdumpable, ASystemException::SystemError, __FILE__, __LINE__);
#define ATHROW_LAST_OS_ERROR_EX(debugdumpable, msg) throw ASystemException(debugdumpable, ASystemException::SystemError, __FILE__, __LINE__, msg);
#define ATHROW_LAST_OS_ERROR_KNOWN(debugdumpable, n) throw ASystemException(n, debugdumpable, __FILE__, __LINE__);
#define ATHROW_LAST_OS_ERROR_KNOWN_EX(debugdumpable, n, msg) throw ASystemException(n, debugdumpable, __FILE__, __LINE__, msg);

#endif

