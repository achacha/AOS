#include "pchABase.hpp"

#include "ASocketLibrary.hpp"
#include "ASocketException.hpp"
#include "ALock.hpp"
#include "ATextConverter.hpp"
#include <Iphlpapi.h>

const AString ASocketLibrary::ANY_ADDRESS("AnyAddress");
const AString ASocketLibrary::BROADCAST("Broadcast");

const AString ASocketLibrary::LOCAL_LOOPBACK("127.0.0.1");

void ASocketLibrary::SocketInfo::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ASocketLibrary::SocketInfo @ " << std::hex << this << std::dec << ") {  "
    << "  m_address=" << m_address
    << "  m_port=" << m_port
    << "  m_handle=0x" << std::hex << m_handle << std::dec
    << "  }" << std::endl;
}

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
    {
      ATHROW_LAST_SOCKET_ERROR(NULL);
    }
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

bool ASocketLibrary::canBindToPort(
  int port, 
  const AString& ip   // = AConstant::ASTRING_EMPTY
)
{
  SOCKET s = ::socket(PF_INET, SOCK_STREAM, 0);
  
  sockaddr_in sadd;
  memset(&sadd, 0, sizeof(sadd));
  sadd.sin_family = AF_INET;
  sadd.sin_port   = (u_short)htons(port);
  
  if (ip.isEmpty())
    sadd.sin_addr.s_addr = INADDR_ANY;
  else
    sadd.sin_addr.s_addr = ::inet_addr(ip.c_str());

  int result = ::bind(s, (sockaddr *)&sadd, sizeof(sadd));

  ::closesocket(s);

  return (result ? false : true);
}

void ASocketLibrary::convertIp4ToString(u4 ip, AOutputBuffer& target)
{
  target.append(AString::fromU1(GET_U4_3(ip)));
  target.append('.');
  target.append(AString::fromU1(GET_U4_2(ip)));
  target.append('.');
  target.append(AString::fromU1(GET_U4_1(ip)));
  target.append('.');
  target.append(AString::fromU1(GET_U4_0(ip)));
}

u4 ASocketLibrary::convertStringToIp4(const AString& ip)
{
  VECTOR_AString parts;
  ip.split(parts, '.');
  AASSERT(NULL, 4 == parts.size()); //a_ Only works with A.B.C.D

  u4 ret = MAKE_U4((u1)parts.at(3).toU4(), (u1)parts.at(2).toU4(), (u1)parts.at(1).toU4(), (u1)parts.at(0).toU4());
  return ret;
}

AString ASocketLibrary::getMACfromIP(const AString& strIP)
{
  const int arraySize = 32;
  AAutoArrayPtr<IP_ADAPTER_INFO> pInfo(new IP_ADAPTER_INFO[arraySize]);
  DWORD dwLen = sizeof(IP_ADAPTER_INFO) * arraySize;

  DWORD ret = GetAdaptersInfo(pInfo.use(), &dwLen);
  switch (ret)
  {
    case ERROR_BUFFER_OVERFLOW:
      ATHROW_EX(NULL, AException::APIFailure, ASWNL("Error allocating memory needed to call GetAdaptersinfo"));

    case ERROR_SUCCESS:
      break;

    default:
      ATHROW_LAST_SOCKET_ERROR(NULL);
  }

  IP_ADAPTER_INFO *p = pInfo.use();
  while (p)
  {
    IP_ADDR_STRING *pIp = &(p->IpAddressList);
    while (pIp)
    {
      if (strIP.equals(pIp->IpAddress.String))
      {
        //a_Build MAC
        AString mac;
        if (p->AddressLength > 1)
        {
          u4 i=0;
          for (; i<p->AddressLength-1; ++i)
          {
            mac.append(ATextConverter::convertBYTEtoHEX(p->Address[i]));
            mac.append('-');
          }
          mac.append(ATextConverter::convertBYTEtoHEX(p->Address[i]));
        }
        return mac;
      }
      pIp = pIp->Next;
    }

    p = p->Next;
  }

  return 0;
}
