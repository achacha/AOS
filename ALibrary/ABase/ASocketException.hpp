#ifndef INCLUDED__ASocketException_HPP__
#define INCLUDED__ASocketException_HPP__

#include "apiABase.hpp"
#include "AException.hpp"

class ABASE_API ASocketException : public AException
{
public:
  /**
    These are exceptions that relate to string manipulation
    string mask: 0x000020XX
  */
  enum SOCKET_EXCEPTION_TYPE
  {
    LastOSSocketError               = 0x00002000,  //a_Calls OS based error checking (if any) and uses that error
    DesiredVersionNotSupported      = 0x00002001,  //a_Desired WinSock version is not supported
    UnableToInitializeSocketLibrary = 0x00002002,  //a_Unable to initialize socket library
    FailedToCreateHandle            = 0x00002003,  //a_Failed to create a valid handle
    InvalidSocket                   = 0x00002004,  //a_Invalid socket handle detected
    InvalidServerName               = 0x00002005,  //a_Invalid server name (usually when empty)
    ReadOperationFailed             = 0x00002006,  //a_Read operation failure
    WriteOperationFailed            = 0x00002007,  //a_Write operation failure
    FailedToAllowLocalAddressReuse  = 0x00002008,  //a_Failed to allow local address reuse
    UnableToConnect                 = 0x00002009   //a_Unable to connect
  };

public:
	/**
    Default construction is of last OS socket error
  */
  ASocketException(
    const ADebugDumpable *const pObject = NULL,
    ASocketException::SOCKET_EXCEPTION_TYPE type = ASocketException::LastOSSocketError, 
    const char* pccFilename = "", 
    int iLineNumber = 0x0, 
    const AString& strExtra = AString::sstr_Empty
  );
  ASocketException(
    int iLastSocketError, 
    const ADebugDumpable *const pObject = NULL,
    const char* pccFilename = "", 
    int iLineNumber = 0x0, 
    const AString& strExtra = AString::sstr_Empty
  );
	
  /**
    Copy ctor
  */
  ASocketException(const ASocketException&);

  /*!
  AException
  */
  virtual void getDescription(AOutputBuffer&) const throw();

private:
  void __getLastWSASocketError(AOutputBuffer&) const throw();
  int m_iLastSocketError;                               //a_Can be provided explicitly if this value was read
};

/*!
Throws a socket exception based on the ::WSAGetLastError()
*/
#define ATHROW_LAST_SOCKET_ERROR(debugdumpable) \
  do { throw ASocketException(::GetLastError(), debugdumpable, __FILE__, __LINE__, AString::sstr_Empty); } while(0x0)
#define ATHROW_LAST_SOCKET_ERROR_KNOWN(debugdumpable, errornum) \
  do { throw ASocketException(errornum, debugdumpable, __FILE__, __LINE__, AString::sstr_Empty); } while(0x0)

#endif

