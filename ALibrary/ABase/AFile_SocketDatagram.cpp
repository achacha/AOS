/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AFile_SocketDatagram.hpp"
#include "ASocketLibrary.hpp"
#include "ASocketException.hpp"

static const int DEFAULT_MAXDATAGRAMSIZE = 0x200;

AFile_SocketDatagram::AFile_SocketDatagram() :
  m_MaximumDatagramSize(DEFAULT_MAXDATAGRAMSIZE)
{
}

AFile_SocketDatagram::~AFile_SocketDatagram()
{
  close();
}

void AFile_SocketDatagram::close()
{
  if (m_SocketInfo.isValid())
  {
    ::closesocket(m_SocketInfo.m_handle);
    m_SocketInfo.m_handle = INVALID_SOCKET;
  }
}

void AFile_SocketDatagram::setReceiveTimeout(size_t miliSec)
{
  _makeHandle();
  ::setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_RCVTIMEO,(const char*) &miliSec, sizeof(int));
}

void AFile_SocketDatagram::setSendTimeout(size_t miliSec)
{
  _makeHandle();
  ::setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_SNDTIMEO,(const char*) &miliSec, sizeof(int));
}

size_t AFile_SocketDatagram::_write(const void *data, size_t length)
{
  m_Buffer.append((const char *)data, length);

  return length;
}

size_t AFile_SocketDatagram::_read(void *data, size_t length)
{
  AString readData;
  size_t bytes = m_Buffer.get(readData, 0x0, length);
  memcpy(data, readData.c_str(), readData.getSize());
  return bytes;
}

size_t AFile_SocketDatagram::receiveFrom(int port, const AString& address)
{
  if (!m_SocketInfo.isValid())
    _makeHandle();

  if (port)
    bind(port, address);

  sockaddr_in sadd;
  memset(&sadd, 0, sizeof(sadd));
  sadd.sin_family = AF_INET;
  
  sadd.sin_port = htons(m_SocketInfo.m_port);
  
  if (address == ASocketLibrary::ANY_ADDRESS)
    sadd.sin_addr.s_addr = INADDR_ANY;
  else
    sadd.sin_addr.s_addr = inet_addr(address.c_str());
  
  int saddLen = sizeof(sockaddr_in);

  m_Buffer.clear();
  
  size_t bytesRead = ::recvfrom(
    m_SocketInfo.m_handle, 
    m_Buffer.startUsingCharPtr(m_MaximumDatagramSize), 
    m_MaximumDatagramSize, 
    0, 
    (sockaddr*)&sadd, 
    &saddLen
  );
  
  m_Buffer.stopUsingCharPtr(bytesRead);
  if (bytesRead == SOCKET_ERROR)
    ATHROW_LAST_SOCKET_ERROR(this);

  return bytesRead;
}

size_t AFile_SocketDatagram::sendTo(int port, const AString& address)
{
  if (!m_SocketInfo.isValid())
    _makeHandle();

//  if (!m_SocketInfo.m_port)
//    bind(portNum, ipAddr);

  int val = 1;
  ::setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_BROADCAST, (const char*) &val, sizeof(int));

  sockaddr_in sadd;
  memset(&sadd, 0, sizeof(sadd));
  sadd.sin_family = AF_INET;
  sadd.sin_port   = (u_short)htons(port);
  if (address == ASocketLibrary::BROADCAST)
    sadd.sin_addr.s_addr = INADDR_BROADCAST;
  else
    sadd.sin_addr.s_addr = inet_addr(address.c_str());

  //a_Send N packets based on max size
  int bytesSent = 0;
  do
  {
    size_t bytesToSend = (m_Buffer.getSize() > m_MaximumDatagramSize ? m_MaximumDatagramSize : m_Buffer.getSize());
    
    size_t bytesLastSent = ::sendto(
      m_SocketInfo.m_handle, 
      m_Buffer.c_str(), 
      bytesToSend, 
      MSG_DONTROUTE, 
      (sockaddr*)&sadd, 
      sizeof(sockaddr_in)
    );
    
    if (bytesLastSent==SOCKET_ERROR)
      ATHROW_LAST_SOCKET_ERROR(this);
    m_Buffer.remove(bytesLastSent);
    bytesSent += bytesLastSent;
  }
  while (m_Buffer.getSize() > m_MaximumDatagramSize);

  return bytesSent;
}

void AFile_SocketDatagram::_makeHandle()
{
  if (!m_SocketInfo.isValid())
  {
    m_SocketInfo.m_handle = socket(PF_INET, SOCK_DGRAM, 0);
    if (!m_SocketInfo.isValid())
      ATHROW(this, ASocketException::FailedToCreateHandle);
  }
}

void AFile_SocketDatagram::bind(int port, const AString& ipAddr)
{
  if (!m_SocketInfo.isValid())
    _makeHandle();

  if ((port != m_SocketInfo.m_port) || (ipAddr != m_SocketInfo.m_address))
  {
    // fill in the address struct
    sockaddr_in sadd;
    memset(&sadd, 0, sizeof(sadd));
    sadd.sin_family = AF_INET;
    sadd.sin_port   = (u_short)htons(port);
    
    if (ipAddr==ASocketLibrary::ANY_ADDRESS)
    	sadd.sin_addr.s_addr = INADDR_ANY;
    else
    	sadd.sin_addr.s_addr = inet_addr(ipAddr.c_str());

    int result = ::bind(m_SocketInfo.m_handle, (sockaddr*)&sadd, sizeof(sadd));
    if (result==SOCKET_ERROR)
      ATHROW_LAST_SOCKET_ERROR(this);

    m_SocketInfo.m_port    = (u2)port;
    m_SocketInfo.m_address = ipAddr;
  }
}

bool AFile_SocketDatagram::isInputWaiting() const
{
  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  fd_set sockSet;
  FD_ZERO(&sockSet);
  FD_SET(m_SocketInfo.m_handle, &sockSet);
  
  return (select(0, &sockSet, NULL, NULL, &timeout) == 0);
}

bool AFile_SocketDatagram::_isNotEof()
{
  //a_For datagrams EOF doesn't make as much sense
  return true;
}

AString& AFile_SocketDatagram::useBuffer()
{ 
  return m_Buffer;
}

size_t AFile_SocketDatagram::getMaximumDatagramSize() const 
{ 
  return m_MaximumDatagramSize; 
}

void AFile_SocketDatagram::setMaximumDatagramSize(size_t size) 
{ 
  m_MaximumDatagramSize = size; 
}
