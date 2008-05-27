/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "ASocketException.hpp"

ASocketException::ASocketException
(
  const ADebugDumpable *const pObject,          // = NULL
  ASocketException::SOCKET_EXCEPTION_TYPE type, // = ASocketException::LastOSSocketError
  const char* pccFilename,                      // = ""
  int iLineNumber,                              // = 0x0
  const AEmittable& extra                       // = AConstant::ASTRING_EMPTY
) :
  AException(pObject, type, pccFilename, iLineNumber, extra),
  m_iLastSocketError(0)
{
}

ASocketException::ASocketException(
  int iLastSocketError, 
  const ADebugDumpable *const pObject, // = NULL
  const char* pccFilename,             // = ""
  int iLineNumber,                     // = 0x0 
  const AEmittable& extra              // = AConstant::ASTRING_EMPTY
) :
  AException(pObject, AException::SocketError, pccFilename, iLineNumber, extra),
  m_iLastSocketError(iLastSocketError)
{
}

ASocketException::ASocketException(const ASocketException &that) :
  AException(that),
  m_iLastSocketError(that.m_iLastSocketError)
{
}

void ASocketException::getDescription(AOutputBuffer& target) const throw()
{
  switch(_getID())
  {
    case DesiredVersionNotSupported:
      target.append("Socket library of desired version cannot be found on this system."); break;
 
    case UnableToInitializeSocketLibrary:
      target.append("Unable to find the socket library."); break;
  
    case FailedToCreateHandle:
      target.append("Failed to create a valid socket handle."); break;

    case InvalidSocket:
      target.append("Cannot operate on an invalid socket handle."); break;

    case InvalidServerName:
      target.append("Invalid/Empty server name."); break;

    case ReadOperationFailed:
      target.append("Read operation failed."); break;

    case WriteOperationFailed:
      target.append("Write operation failed."); break;

    case FailedToAllowLocalAddressReuse:
      target.append("Failed to allow local address reuse."); break;

    case UnableToConnect:
      target.append("Unable to connect."); break;

    case SocketError:
      __getLastWSASocketError(target); break;
    
    default:
      AException::getDescription(target); break;
  }
}

#ifdef __WINDOWS__
void ASocketException::__getLastWSASocketError(AOutputBuffer& target) const throw()
{
  u4 error_number;
  if (m_iLastSocketError > 0)
  {
    error_number = m_iLastSocketError;
  }
  else
  {
    error_number = ::WSAGetLastError();
  }

  switch(error_number)
  {
	  case WSAEACCES :
	    target.append("Permission denied.\r\n An attempt was made to access a socket in a way forbidden by its access permissions. An example is using a broadcast address for sendto without broadcast permission being set using setsockopt(SO_BROADCAST)."); break;
	  case WSAEADDRINUSE :
	    target.append("Address already in use.\r\n Only one usage of each socket address (protocol/IP address/port) is normally permitted. This error occurs if an application attempts to bind a socket to an IP address/port that has already been used for an existing socket, or a socket that wasn't closed properly, or one that is still in the process of closing. For server applications that need to bind multiple sockets to the same port number, consider using setsockopt(SO_REUSEADDR). Client applications usually need not call bind at all - connect will choose an unused port automatically. When bind is called with a wild-card address (involving ADDR_ANY), a WSAEADDRINUSE error could be delayed until the specific address is 'committed.'; This could happen with a call to other function later, including connect, listen, WSAConnect or WSAJoinLeaf."); break;
	  case WSAEADDRNOTAVAIL :
	    target.append("Cannot assign requested address.\r\n The requested address is not valid in its context. Normally results from an attempt to bind to an address that is not valid for the local machine. This can also result from connect, sendto, WSAConnect, WSAJoinLeaf, or WSASendTo when the remote address or port is not valid for a remote machine (e.g. address or port 0)."); break;
	  case WSAEAFNOSUPPORT :
	    target.append("Address family not supported by protocol family.\r\n An address incompatible with the requested protocol was used. All sockets are created with an associated 'address family' (i.e. AF_INET for Internet Protocols) and a generic protocol type (i.e. SOCK_STREAM). This error will be returned if an incorrect protocol is explicitly requested in the socket call, or if an address of the wrong family is used for a socket, e.g. in sendto."); break;
	  case WSAEALREADY :
	    target.append("Operation already in progress.\r\n An operation was attempted on a non-blocking socket that already had an operation in progress - i.e. calling connect a second time on a non-blocking socket that is already connecting, or canceling an asynchronous request (WSAAsyncGetXbyY) that has already been canceled or completed."); break;
	  case WSAECONNABORTED :
	    target.append("Software caused connection abort.\r\n An established connection was aborted by the software in your host machine, possibly due to a data transmission timeout or protocol error."); break;
	  case WSAECONNREFUSED :
	    target.append("Connection refused.\r\n No connection could be made because the target machine actively refused it. This usually results from trying to connect to a service that is inactive on the foreign host - i.e. one with no server application running."); break;
	  case WSAECONNRESET :
	    target.append("Connection reset by peer.\r\n A existing connection was forcibly closed by the remote host. This normally results if the peer application on the remote host is suddenly stopped, the host is rebooted, or the remote host used a 'hard close' (see setsockopt for more information on the SO_LINGER option on the remote socket.) This error may also result if a connection was broken due to 'keep-alive' activity detecting a failure while one or more operations are in progress. Operations that were in progress fail with WSAENETRESET. Subsequent operations fail with WSAECONNRESET."); break;
	  case WSAEDESTADDRREQ :
	    target.append("Destination address required.\r\n A required address was omitted from an operation on a socket. For example, this error will be returned if sendto is called with the remote address of ADDR_ANY."); break;
	  case WSAEFAULT :
	    target.append("Bad address.\r\n The system detected an invalid pointer address in attempting to use a pointer argument of a call. This error occurs if an application passes an invalid pointer value, or if the length of the buffer is too small. For instance, if the length of an argument which is a struct sockaddr is smaller than sizeof(struct sockaddr)."); break;
	  case WSAEHOSTDOWN :
	    target.append("Host is down.\r\n A socket operation failed because the destination host was down. A socket operation encountered a dead host. Networking activity on the local host has not been initiated. These conditions are more likely to be indicated by the error WSAETIMEDOUT."); break;
	  case WSAEHOSTUNREACH :
	    target.append("No route to host.\r\n A socket operation was attempted to an unreachable host. See WSAENETUNREACH."); break;
	  case WSAEINPROGRESS :
	    target.append("Operation now in progress.\r\n A blocking operation is currently executing. Windows Sockets only allows a single blocking operation to be outstanding per task (or thread), and if any other function call is made (whether or not it references that or any other socket) the function fails with the WSAEINPROGRESS error."); break;
	  case WSAEINTR :
	    target.append("Interrupted function call.\r\n A blocking operation was interrupted by a call to WSACancelBlockingCall."); break;
	  case WSAEINVAL :
	    target.append("Invalid argument.\r\n Some invalid argument was supplied (for example, specifying an invalid level to the setsockopt function). In some instances, it also refers to the current state of the socket - for instance, calling accept on a socket that is not listening."); break;
	  case WSAEISCONN :
	    target.append("Socket is already connected.\r\n A connect request was made on an already connected socket. Some implementations also return this error if sendto is called on a connected SOCK_DGRAM socket (For SOCK_STREAM sockets, the to parameter in sendto is ignored), although other implementations treat this as a legal occurrence."); break;
	  case WSAEMFILE :
	    target.append("Too many open files.\r\n Too many open sockets. Each implementation may have a maximum number of socket handles available, either globally, per process or per thread."); break;
	  case WSAEMSGSIZE :
	    target.append("Message too long.\r\n A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself."); break;
	  case WSAENETDOWN :
	    target.append("Network is down.\r\n A socket operation encountered a dead network. This could indicate a serious failure of the network system (i.e. the protocol stack that the WinSock DLL runs over), the network interface, or the local network itself."); break;
	  case WSAENETRESET :
	    target.append("Network dropped connection on reset.\r\n The connection has been broken due to 'keep-alive' activity detecting a failure while the operation was in progress. It can also be returned by setsockopt if an attempt is made to set SO_KEEPALIVE on a connection that has already failed."); break;
	  case WSAENETUNREACH :
	    target.append("Network is unreachable.\r\n A socket operation was attempted to an unreachable network. This usually means the local software knows no route to reach the remote host."); break;
	  case WSAENOBUFS :
	    target.append("No buffer space available.\r\n An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full."); break;
	  case WSAENOPROTOOPT :
	    target.append("Bad protocol option.\r\n An unknown, invalid or unsupported option or level was specified in a getsockopt or setsockopt call."); break;
	  case WSAENOTCONN :
	    target.append("Socket is not connected.\r\n A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using sendto) no address was supplied. Any other type of operation might also return this error - for example, setsockopt setting SO_KEEPALIVE if the connection has been reset."); break;
	  case WSAENOTSOCK :
	    target.append("Socket operation on non-socket.\r\n An operation was attempted on something that is not a socket. Either the socket handle parameter did not reference a valid socket, or for select, a member of an fd_set was not valid."); break;
	  case WSAEOPNOTSUPP :
	    target.append("Operation not supported.\r\n The attempted operation is not supported for the type of object referenced. Usually this occurs when a socket descriptor to a socket that cannot support this operation, for example, trying to accept a connection on a datagram socket."); break;
	  case WSAEPFNOSUPPORT :
	    target.append("Protocol family not supported.\r\n The protocol family has not been configured into the system or no implementation for it exists. Has a slightly different meaning to WSAEAFNOSUPPORT, but is interchangeable in most cases, and all Windows Sockets functions that return one of these specify WSAEAFNOSUPPORT."); break;
	  case WSAEPROCLIM :
	    target.append("Too many processes.\r\n A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously. WSAStartup may fail with this error if the limit has been reached."); break;
	  case WSAEPROTONOSUPPORT :
	    target.append("Protocol not supported.\r\n The requested protocol has not been configured into the system, or no implementation for it exists. For example, a socket call requests a SOCK_DGRAM socket, but specifies a stream protocol."); break;
	  case WSAEPROTOTYPE :
	    target.append("Protocol wrong type for socket.\r\n A protocol was specified in the socket function call that does not support the semantics of the socket type requested. For example, the ARPA Internet UDP protocol cannot be specified with a socket type of SOCK_STREAM."); break;
	  case WSAESHUTDOWN :
	    target.append("Cannot send after socket shutdown.\r\n A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call. By calling shutdown a partial close of a socket is requested, which is a signal that sending or receiving or both has been discontinued."); break;
	  case WSAESOCKTNOSUPPORT :
	    target.append("Socket type not supported.\r\n The support for the specified socket type does not exist in this address family. For example, the optional type SOCK_RAW might be selected in a socket call, and the implementation does not support SOCK_RAW sockets at all."); break;
	  case WSAETIMEDOUT :
	    target.append("Connection timed out.\r\n A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond."); break;
	  case WSATYPE_NOT_FOUND :
	    target.append("Class type not found.\r\n The specified class was not found."); break;
	  case WSAEWOULDBLOCK :
	    target.append("Resource temporarily unavailable.\r\n This error is returned from operations on non-blocking sockets that cannot be completed immediately, for example recv when no data is queued to be read from the socket. It is a non-fatal error, and the operation should be retried later. It is normal for WSAEWOULDBLOCK to be reported as the result from calling connect on a non-blocking SOCK_STREAM socket, since some time must elapse for the connection to be established."); break;
	  case WSAHOST_NOT_FOUND :
	    target.append("Host not found.\r\n No such host is known. The name is not an official hostname or alias, or it cannot be found in the database(s) being queried. This error may also be returned for protocol and service queries, and means the specified name could not be found in the relevant database."); break;
#if !defined(_WIN64) && defined(_WIN32)	  
    case WSA_INVALID_HANDLE :
	    target.append("Specified event object handle is invalid.\r\n An application attempts to use an event object, but the specified handle is not valid."); break;
    case WSA_INVALID_PARAMETER :
	    target.append("One or more parameters are invalid.\r\n An application used a Windows Sockets function which directly maps to a Win32 function. The Win32 function is indicating a problem with one or more parameters."); break;
	  case WSA_IO_PENDING :
	    target.append("Overlapped operations will complete later.\r\n The application has initiated an overlapped operation which cannot be completed immediately. A completion indication will be given at a later time when the operation has been completed."); break;
	  case WSA_IO_INCOMPLETE :
	    target.append("Overlapped I/O event object not in signaled state.\r\n The application has tried to determine the status of an overlapped operation which is not yet completed. Applications that use WSAGetOverlappedResult (with the fWait flag set to false) in a polling mode to determine when an overlapped operation has completed will get this error code until the operation is complete."); break;
	  case WSA_NOT_ENOUGH_MEMORY :
	    target.append("Insufficient memory available.\r\n An application used a Windows Sockets function which directly maps to a Win32 function. The Win32 function is indicating a lack of required memory resources."); break;
	  case WSA_OPERATION_ABORTED :
	    target.append("Overlapped operation aborted.\r\n An overlapped operation was canceled due to the closure of the socket, or the execution of the SIO_FLUSH command in WSAIoctl."); break;
#endif 
	  case WSANOTINITIALISED :
	    target.append("Successful WSAStartup not yet performed (forgot global a ASocketLibrary object?).\r\n Either the application hasn't called WSAStartup or WSAStartup failed. The application may be accessing a socket which the current active task does not own (i.e. trying to share a socket between tasks), or WSACleanup has been called too many times. Create a global instance of ASocketLibrary object that will initialize sockets upon creation and de-initialize them when destroyed."); break;
	  case WSANO_DATA :
	    target.append("Valid name, no data record of requested type.\r\n The requested name is valid and was found in the database, but it does not have the correct associated data being resolved for. The usual example for this is a hostname -> address translation attempt (using gethostbyname or WSAAsyncGetHostByName) which uses the DNS (Domain Name Server), and an MX record is returned but no A record - indicating the host itself exists, but is not directly reachable."); break;
	  case WSANO_RECOVERY :
	    target.append("This is a non-recoverable error.\r\n This indicates some sort of non-recoverable error occurred during a database lookup. This may be because the database files (e.g. BSD-compatible HOSTS, SERVICES or PROTOCOLS files) could not be found, or a DNS request was returned by the server with a severe error."); break;
	  case WSASYSCALLFAILURE :
	    target.append("System call failure.\r\n Returned when a system call that should never fail does. For example, if a call to WaitForMultipleObjects fails or one of the registry functions fails trying to manipulate the protocol/namespace catalogs."); break;
	  case WSASYSNOTREADY :
	    target.append("Network subsystem is unavailable.\r\n This error is returned by WSAStartup if the Windows Sockets implementation cannot function at this time because the underlying system it uses to provide network services is currently unavailable. Users should check: that the appropriate Windows Sockets DLL file is in the current path, that they are not trying to use more than one Windows Sockets implementation simultaneously. If there is more than one WINSOCK DLL on your system, be sure the first one in the path is appropriate for the network subsystem currently loaded. the Windows Sockets implementation documentation to be sure all necessary components are currently installed and configured correctly."); break;
	  case WSATRY_AGAIN :
	    target.append("Non-authoritative host not found.\r\n This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server. A retry at some time later may be successful."); break;
	  case WSAVERNOTSUPPORTED :
	    target.append("WINSOCK.DLL version out of range.\r\n The current Windows Sockets implementation does not support the Windows Sockets specification version requested by the application. Check that no old Windows Sockets DLL files are being accessed."); break;
	  case WSAEDISCON :
	    target.append("Graceful shutdown in progress.\r\n Returned by WSARecv and WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence."); break;
    default:
        target.append("Unknown error."); break;
  }
}
#else
void ASocketException::__getLastWSASocketError(AOutputBuffer& target) const throw()
{
  u4 error_number;
  if (m_iLastSocketError > 0)
  {
    error_number = m_iLastSocketError;
  }
  else
  {
    error_number = errno;
  }

  target.append(ASW("errno=",6));
  target.append(AString::fromU4(error_number));
}
#endif
