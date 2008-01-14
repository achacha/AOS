
#include "pchABase.hpp"

#include "AFile_Socket.hpp"
#include "ASocketException.hpp"
#include "ASocketLibrary.hpp"
#include "ASocketListener.hpp"
#include "AHTTPRequestHeader.hpp"

void AFile_Socket::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AFile_Socket @ " << std::hex << this << std::dec << ") {" << std::endl;
  AFile::debugDump(os,indent+1);
  ADebugDumpable::indent(os, indent+1) << "m_SocketInfo=" << std::endl;
  m_SocketInfo.debugDump(os,indent+2);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFile_Socket::AFile_Socket(
  const AString& ip, 
  int port,      
  bool blocking // = false
) :
  mbool_Blocking(blocking),
  mp_Listener(NULL),
  m_EOF(true)
{
  m_SocketInfo.m_address = ip;
  m_SocketInfo.m_port = port;
}

AFile_Socket::AFile_Socket(
  const AHTTPRequestHeader& request, 
  bool blocking // = false
) :
  mbool_Blocking(blocking),
  mp_Listener(NULL),
  m_EOF(true)
{
  m_SocketInfo.m_address = ASocketLibrary::getIPFromAddress(request.getUrl().getServer());
  m_SocketInfo.m_port = request.getUrl().getPort();
}

AFile_Socket::AFile_Socket(
  ASocketListener &listenSock, 
  bool blocking // = false
) :
  mbool_Blocking(blocking),
  mp_Listener(&listenSock),
  m_EOF(true)
{
}

AFile_Socket::~AFile_Socket()
{
  close();
}

void AFile_Socket::makeHandle()
{
  if (!m_SocketInfo.isValid())
  {
    m_SocketInfo.m_handle = ::socket(PF_INET, SOCK_STREAM, 0);
    if (!m_SocketInfo.isValid())
      ATHROW_LAST_SOCKET_ERROR(this);
  }
}

void AFile_Socket::open()
{
  if (m_SocketInfo.isValid())
    close();

  if (mp_Listener)
  {
    m_SocketInfo = mp_Listener->accept();
  }
  else
  {
    //a_Make sure we have a valid socket handle
    makeHandle();

    //a_Make sure the address provided is a valid IP number fill in the address struct
    sockaddr_in sadd;
    memset(&sadd, 0, sizeof(sadd));
    sadd.sin_family = AF_INET;
    sadd.sin_port   = (u_short)htons(m_SocketInfo.m_port);
    sadd.sin_addr.s_addr = inet_addr(m_SocketInfo.m_address.c_str());

    //a_Connect
    int result = ::connect(m_SocketInfo.m_handle, (sockaddr *)&sadd, sizeof(sadd));
    if (result!=0)
    {
      int lastError = ::WSAGetLastError();
      if (mbool_Blocking || lastError != WSAEWOULDBLOCK)
      {
        m_SocketInfo.m_handle = INVALID_SOCKET;
        ATHROW_LAST_SOCKET_ERROR_KNOWN_EX(this, lastError, m_SocketInfo);
      }
    }
  }

  //a_Set to non-blocking if required.
  if (!mbool_Blocking) 
  {
    setNonBlocking();
  }

  m_EOF = false;
}

void AFile_Socket::close()
{
  if (m_SocketInfo.isValid())
  {
    ::closesocket(m_SocketInfo.m_handle);
    m_SocketInfo.m_handle = INVALID_SOCKET;
  }
  m_EOF = true;
}

const ASocketLibrary::SocketInfo& AFile_Socket::getSocketInfo() const
{
  return m_SocketInfo;
}

void AFile_Socket::setCloseTimeout(size_t miliSec)
{
  makeHandle();

  struct 
  { 
    int    l_onoff; 
    int    l_linger; 
  } _linger;

  _linger.l_onoff = 1;
  _linger.l_linger = (int)miliSec;
    
  setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_LINGER,(const char*) &_linger, sizeof(_linger));
}

void AFile_Socket::setReadTimeout(size_t miliSec)
{
  makeHandle();
  setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_RCVTIMEO,(const char*) &miliSec, sizeof(int));
}

void AFile_Socket::setWriteTimeout(size_t miliSec)
{
  makeHandle();
  setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_SNDTIMEO,(const char*) &miliSec, sizeof(int));
}

size_t AFile_Socket::_write(const void *buf, size_t size)
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  if (size <= 0x0)
	  return AConstant::npos;

  size_t bytesToWrite = size;
  size_t totalBytesWritten = 0;

  //a_It may take multiple calls to send the entire buffer
  while (bytesToWrite)
  {
    size_t bytesWritten = ::send(m_SocketInfo.m_handle, (const char *) buf + totalBytesWritten, bytesToWrite, 0);

    if (!bytesWritten && bytesToWrite)
      ATHROW_LAST_SOCKET_ERROR_KNOWN(this, ASocketException::WriteOperationFailed);
      
    if (bytesWritten == SOCKET_ERROR)
      ATHROW_LAST_SOCKET_ERROR(this);

    AASSERT(this, bytesToWrite >= bytesWritten);
    bytesToWrite -= bytesWritten;      //reduce bytes to write
    totalBytesWritten += bytesWritten; //increase offset for buffer
  }

  return totalBytesWritten;
}

size_t AFile_Socket::readBlocking(void *buf, size_t size)
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  if (!size)
    return 0;
  
  size_t bytesToRead = size;
  size_t totalBytesRead = 0;

  //a_It may take multiple calls to read the entire buffer
  while (bytesToRead)
  {
    size_t bytesReceived = ::recv(m_SocketInfo.m_handle, (char *) buf + totalBytesRead, bytesToRead, 0 );

    if (!bytesReceived)
      ATHROW_LAST_SOCKET_ERROR_KNOWN(this, ASocketException::ReadOperationFailed);

    if (bytesReceived == SOCKET_ERROR)
      ATHROW_LAST_SOCKET_ERROR(this);

    AASSERT(this, bytesToRead >= bytesReceived);
    bytesToRead -= bytesReceived; //reduce bytes to read
    totalBytesRead += bytesReceived; //increase offset for buffer
  }

  return totalBytesRead;
}

size_t AFile_Socket::_read(void *buf, size_t size)
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  if (m_EOF)
    return 0;

  if (mbool_Blocking)
    return readBlocking(buf, size);
  
  //a_Cannot read zero bytes, return of 0 means EOF
  if (!size)
    ATHROW_LAST_SOCKET_ERROR_KNOWN(this, ASocketException::InvalidParameter);
  
  size_t bytesReceived = ::recv(m_SocketInfo.m_handle, (char *) buf, size, 0);
    
  if (bytesReceived == SOCKET_ERROR)
  {
    int err = ::WSAGetLastError();
    if (err == WSAEWOULDBLOCK)
      return AConstant::npos; //Operation would block so return not found

    ATHROW_LAST_SOCKET_ERROR_KNOWN(this, err);
  }
  
  AASSERT(this, bytesReceived >= 0);
  return bytesReceived;
}

bool AFile_Socket::isInputWaiting() const
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  fd_set sockSet;
  FD_ZERO(&sockSet);
  FD_SET(m_SocketInfo.m_handle, &sockSet);
  
  int ret = select(0, &sockSet, NULL, NULL, &timeout);
  if (SOCKET_ERROR == ret)
    ATHROW_LAST_SOCKET_ERROR(this);

  return (ret > 0);
}

bool AFile_Socket::isOutputBlocked() const
{
  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  fd_set sockSet;
  FD_ZERO(&sockSet);
  FD_SET(m_SocketInfo.m_handle, &sockSet);
  
  return (select(0, NULL, &sockSet, NULL, &timeout) == 0);
}

void AFile_Socket::setNonBlocking()
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  mbool_Blocking = false;
  u4 mode = 1;
  if (ioctlsocket(m_SocketInfo.m_handle, FIONBIO, &mode))
    ATHROW_LAST_SOCKET_ERROR(this);
}

void AFile_Socket::setBlocking()
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  mbool_Blocking = true;
  u4 mode = 0;
  if (ioctlsocket(m_SocketInfo.m_handle, FIONBIO, &mode))
    ATHROW_LAST_SOCKET_ERROR(this);
}

size_t AFile_Socket::_readBlockIntoLookahead()
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  if (m_EOF)
    return 0;

  AString str;
  char *p = str.startUsingCharPtr(AFile::DefaultReadBlock);
  
  size_t bytesRead = 0;
  if (mbool_Blocking)
  {
    //a_Blocking mode, only read what we have available to prevent blocking
    size_t bytesToRead = _availableInputWaiting();
    if (bytesToRead > m_ReadBlock)
      bytesToRead = m_ReadBlock;
    if (bytesToRead > 0)
      bytesRead = _read(p, bytesToRead);
  }
  else
  {
    //a_Non-blocking, read up to the default read block
    bytesRead = _read(p, m_ReadBlock);
  }
  if (AConstant::npos != bytesRead)
    m_LookaheadBuffer.pushBack(p, bytesRead);
  
  return bytesRead;
}

size_t AFile_Socket::getTotalBytesAvailableForRead() const
{ 
  return _availableInputWaiting() + m_LookaheadBuffer.getSize();
}

size_t AFile_Socket::_availableInputWaiting() const
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  u4 bytes = 1;
  if (ioctlsocket(m_SocketInfo.m_handle, FIONREAD , &bytes))
    ATHROW_LAST_SOCKET_ERROR(this);

  return bytes;
}

bool AFile_Socket::isOpen()
{
#pragma message("AFile_Socket::isOpen: There is no real way of doing this, maybe should return true if handle is valid")
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  fd_set sockSet;
  FD_ZERO(&sockSet);
  FD_SET(m_SocketInfo.m_handle, &sockSet);
  
  return (select(0, NULL, NULL, &sockSet, &timeout) == 0);
}

bool AFile_Socket::_isNotEof()
{
  return !m_EOF;
}

void AFile_Socket::setNagleAlgorithm(bool mode /* = true */)
{
  AASSERT(this, INVALID_SOCKET != m_SocketInfo.m_handle);
  BOOL bOptionVal = mode ? TRUE : FALSE;
  int bOptionLen = sizeof(BOOL);
  if (SOCKET_ERROR == setsockopt(m_SocketInfo.m_handle, IPPROTO_TCP, TCP_NODELAY, (char*)&bOptionVal, bOptionLen))
    ATHROW_LAST_SOCKET_ERROR(this);
}

