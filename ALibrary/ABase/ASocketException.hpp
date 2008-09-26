/*
Written by Alex Chachanashvili

$Id$
*/
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
    LastOSSocketError               = 0x00002000,  //!< Calls OS based error checking (if any) and uses that error
    DesiredVersionNotSupported      = 0x00002001,  //!< Desired WinSock version is not supported
    UnableToInitializeSocketLibrary = 0x00002002,  //!< Unable to initialize socket library
    FailedToCreateHandle            = 0x00002003,  //!< Failed to create a valid handle
    InvalidSocket                   = 0x00002004,  //!< Invalid socket handle detected
    InvalidServerName               = 0x00002005,  //!< Invalid server name (usually when empty)
    ReadOperationFailed             = 0x00002006,  //!< Read operation failure
    WriteOperationFailed            = 0x00002007,  //!< Write operation failure
    FailedToAllowLocalAddressReuse  = 0x00002008,  //!< Failed to allow local address reuse
    UnableToConnect                 = 0x00002009   //!< Unable to connect
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
    const AEmittable& extra = AConstant::ASTRING_EMPTY
  );
  ASocketException(
    int iLastSocketError, 
    const ADebugDumpable *const pObject = NULL,
    const char* pccFilename = "", 
    int iLineNumber = 0x0, 
    const AEmittable& extra = AConstant::ASTRING_EMPTY
  );
	
  /**
    Copy ctor
  */
  ASocketException(const ASocketException&);

  /*!
  AException

  @param target appended the description
  */
  virtual void getDescription(AOutputBuffer& target) const throw();

private:
  void _getLastWSASocketError(AOutputBuffer&) const throw();
  int m_iLastSocketError;
};

/*!
Throws a socket exception based on the ::WSAGetLastError()

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@throw AException always
*/
#define ATHROW_LAST_SOCKET_ERROR(debugdumpable) throw ASocketException(::GetLastError(), debugdumpable, __FILE__, __LINE__, AConstant::ASTRING_EMPTY);

/*!
Throws a socket exception given an error number

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param errornum OS specific error number (from ::WSAGetLastError() or similar) or errno
@throw AException always
*/
#define ATHROW_LAST_SOCKET_ERROR_KNOWN(debugdumpable, errornum) throw ASocketException(errornum, debugdumpable, __FILE__, __LINE__, AConstant::ASTRING_EMPTY);

/*!
Throws a socket exception given an error number with an extra message

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param errornum OS specific error number (from ::WSAGetLastError() or similar) or errno
@param extra text to throw with exception
@throw AException always
*/
#define ATHROW_LAST_SOCKET_ERROR_KNOWN_EX(debugdumpable, errornum, extra) throw ASocketException(errornum, debugdumpable, __FILE__, __LINE__, extra);

#endif

