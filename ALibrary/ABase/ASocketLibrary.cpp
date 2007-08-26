#include "pchABase.hpp"

#include "ASocketLibrary.hpp"
#include "ASocketException.hpp"
#include "ALock.hpp"

const AString ASocketLibrary::ANY_ADDRESS("AnyAddress");
const AString ASocketLibrary::BROADCAST("Broadcast");

const AString ASocketLibrary::LOCAL_LOOPBACK("127.0.0.1");

#ifdef __DEBUG_DUMP__
void ASocketLibrary::SocketInfo::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ASocketLibrary::SocketInfo @ " << std::hex << this << std::dec << ") {  "
    << "  m_address=" << m_address
    << "  m_port=" << m_port
    << "  m_handle=0x" << std::hex << m_handle << std::dec
    << "  }" << std::endl;
}
#endif

ASocketLibrary::ASocketLibrary()
{
#if defined(__WINDOWS__)
  u2 wVersionRequested;  
  WSADATA wsaData; 
  int err; 
  wVersionRequested = MAKEWORD(1, 1); 
 
  err = WSAStartup(wVersionRequested, &wsaData); 
 
  if (err != 0) 
      /* Tell the user that we couldn't find a useable */ 
      /* winsock.dll.     */ 
      return; 
 
  /* Confirm that the Windows Sockets DLL supports 1.1.*/ 
  /* Note that if the DLL supports versions greater */ 
  /* than 1.1 in addition to 1.1, it will still return */ 
  /* 1.1 in wVersion since that is the version we */ 
  /* requested. */ 
  if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 )
  { 
      //a_Could not find winsock.dll
      WSACleanup(); 
      ATHROW(NULL, ASocketException::DesiredVersionNotSupported);
  } 
#endif
}

ASocketLibrary::~ASocketLibrary()
{
#if defined(__WINDOWS__)
  WSACleanup();                     //a_Non zero may mean an error
#endif
}

AString ASocketLibrary::getLocalHostName()
{
  char name[255];
  if (gethostname(name, (int) sizeof(name)))
    ATHROW_LAST_SOCKET_ERROR(NULL);

  return name;
}

VECTOR_AString ASocketLibrary::getHostAddresses(const AString &hostName)
{
  VECTOR_AString result;

#if defined(__WINDOWS__)
  hostent* pHostInfo = gethostbyname(hostName.c_str());
  if (!pHostInfo)
    ATHROW_LAST_SOCKET_ERROR(NULL);
#endif

  for (char**p=pHostInfo->h_addr_list; *p; p++)
    result.push_back(inet_ntoa(*((in_addr*)*p)));

  return result;
}

AString ASocketLibrary::getDefaultIp() 
{
  return *getHostAddresses(getLocalHostName()).begin();
}

AString ASocketLibrary::getIPFromAddress(const AString &address)
{
  // check to see whether this is already an IP number
  unsigned long addr = inet_addr(address.c_str());
  if(addr == INADDR_NONE)
  {
    // address doesn't contain a valid IP number, perhaps it's a hostname
    // get the first address found for it
    VECTOR_AString IPlist = getHostAddresses(address);
    if( IPlist.empty() )
      ATHROW_LAST_SOCKET_ERROR(NULL);

    const AString& aIP = IPlist.front();
    addr = inet_addr(aIP.c_str());
    if( addr == INADDR_NONE )
      ATHROW_LAST_SOCKET_ERROR(NULL);
    else
      return aIP;
  }
  // else it's a valid address, so just pass it back
  return address;
}

ASocketLibrary::SocketInfo::SocketInfo() :
  m_port(0xffff),
  m_handle(INVALID_SOCKET)
{
}

ASocketLibrary::SocketInfo::SocketInfo(const ASocketLibrary::SocketInfo& that) :
  m_port(that.m_port),
  m_handle(that.m_handle),
  m_address(that.m_address)
{
}

ASocketLibrary::SocketInfo& ASocketLibrary::SocketInfo::operator=(const ASocketLibrary::SocketInfo& that)
{
  m_port = that.m_port;
  m_handle = that.m_handle;
  m_address = that.m_address;
  return *this;
}

bool ASocketLibrary::SocketInfo::isValid()
{
  return (m_handle == INVALID_SOCKET ? false : true);
}

bool ASocketLibrary::canBindToPort(int port)
{
  SOCKET s = ::socket(PF_INET, SOCK_STREAM, 0);
  
  sockaddr_in sadd;
  memset(&sadd, 0, sizeof(sadd));
  sadd.sin_family = AF_INET;
  sadd.sin_port   = (u_short)htons(port);
  sadd.sin_addr.s_addr = INADDR_ANY;

  int result = ::bind(s, (sockaddr *)&sadd, sizeof(sadd));

  ::closesocket(s);

  return (result ? false : true);
}
