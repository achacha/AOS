#include "pchABase.hpp"

#include "ASocketListener.hpp"
#include "ASocketException.hpp"
#include "AFile_Socket.hpp"
#include "ASocketLibrary.hpp"

#ifdef __DEBUG_DUMP__
void ASocketListener::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ASocketListener @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_AllowReuse=" << (m_AllowReuse ? AString::sstr_True : AString::sstr_False) << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ASocketListener::ASocketListener(
  int port, 
  const AString& ip,      // = AString::sstr_Empty
  int backlog,            // = 42
  bool allowLocalReuse    // = false
) :
  m_backlog(backlog),
  m_AllowReuse(allowLocalReuse)
{
  m_SocketInfo.m_address = ip;
  m_SocketInfo.m_port = port;
}

ASocketListener::~ASocketListener()
{
  close();
}

void ASocketListener::makeHandle()
{
  if (!m_SocketInfo.isValid())
  {
    m_SocketInfo.m_handle = socket(PF_INET, SOCK_STREAM, 0);
    if (!m_SocketInfo.isValid()) {
      ATHROW_LAST_SOCKET_ERROR(this);
    }

  // Allow local address reuse
  if (m_AllowReuse)
  {
    int optbuffer=1000;
    if (::setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_REUSEADDR, (char *)&optbuffer, sizeof(optbuffer)) )
      ATHROW(this, ASocketException::FailedToAllowLocalAddressReuse);
    }
  }
}

void ASocketListener::open()
{
  // create handle if it hasn't already been created
  makeHandle();

  // fill in the address struct
  sockaddr_in sadd;
  memset(&sadd, 0, sizeof(sadd));
  sadd.sin_family         = AF_INET;
  sadd.sin_port           = (u_short)htons(m_SocketInfo.m_port);
  if (m_SocketInfo.m_address.isEmpty())
  {
    sadd.sin_addr.s_addr = INADDR_ANY;
    m_SocketInfo.m_address.assign(ASW("0.0.0.0",7));
  }
  else
    sadd.sin_addr.s_addr = inet_addr(m_SocketInfo.m_address.c_str());
  
  int result = ::bind(m_SocketInfo.m_handle, (sockaddr *)&sadd, sizeof(sadd));
  if (result!=0)
    ATHROW_LAST_SOCKET_ERROR(this);

  int saddSize = sizeof(sadd);
  if (::getsockname(m_SocketInfo.m_handle, (sockaddr *)&sadd, &saddSize)==0)
    m_SocketInfo.m_address = inet_ntoa(sadd.sin_addr);

  if (m_backlog < 1)
    m_backlog = 1;
  
  result = ::listen(m_SocketInfo.m_handle, m_backlog);
  if (result!=0)
    ATHROW_LAST_SOCKET_ERROR(this);
}

void ASocketListener::close()
{
  if (!m_SocketInfo.isValid())
  {
    ::closesocket(m_SocketInfo.m_handle);
    m_SocketInfo.m_handle = INVALID_SOCKET;
  }
}

ASocketLibrary::SocketInfo ASocketListener::accept()
{
  struct sockaddr_in sadd;
  int saddSize = sizeof(sockaddr_in);

  ASocketLibrary::SocketInfo info;
  info.m_handle  = ::accept(m_SocketInfo.m_handle, (sockaddr*)&sadd, &saddSize);
  if (!info.isValid())
    ATHROW(this, ASocketException::UnableToConnect);

  info.m_port    = ntohs(sadd.sin_port);
  info.m_address = inet_ntoa(sadd.sin_addr);

  return info;
}

void ASocketListener::setReadTimeout(s4 miliSec)
{
  // create handle if it hasnt already been created
  makeHandle();

  setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_RCVTIMEO,(const char*) &miliSec, sizeof(int));
}

void ASocketListener::setWriteTimeout(s4 miliSec)
{
  // create handle if it hasnt already been created
  makeHandle();

  setsockopt(m_SocketInfo.m_handle, SOL_SOCKET, SO_SNDTIMEO,(const char*) &miliSec, sizeof(int));
}

bool ASocketListener::isAcceptWaiting()
{
  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  fd_set sockSet;
  FD_ZERO(&sockSet);
  FD_SET(m_SocketInfo.m_handle, &sockSet);
  
  int ret = select(0, &sockSet, NULL, NULL, &timeout);
  return (ret > 0 ? true : false);
}

const ASocketLibrary::SocketInfo& ASocketListener::getSocketInfo() const
{
  return m_SocketInfo;
}
