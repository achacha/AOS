/*
Written by Alex Chachanashvili

$Id: ASocketLibrary.hpp 205 2008-05-29 13:58:16Z achacha $
*/
#ifndef INCLUDED__ASocketLibrary_HPP__
#define INCLUDED__ASocketLibrary_HPP__

#include "apiAPlatform.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"

/*!
This object must exist to use sockets
Making it a global object in scope of application of shared object/dll will work
(see example at the end of this file)
*/
class APLATFORM_API ASocketLibrary
{
public:
  static const AString ANY_ADDRESS;
  static const AString BROADCAST;
  static const AString LOCAL_LOOPBACK;

  //a_Class to store socket informatioon
  class APLATFORM_API SocketInfo : public ADebugDumpable
  {
  public:
    AString m_address;
    int     m_port;
    u8      m_handle;

    SocketInfo();
    SocketInfo(const SocketInfo& that);
    SocketInfo& operator=(const SocketInfo& that);
    bool isValid();

    /*!
    ADebugDumpable
    */
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  };

public:
  ASocketLibrary();
  ~ASocketLibrary();

  //a_Utility functions
  static AString getLocalHostName();
  static VECTOR_AString getHostAddresses(const AString &hostName);
  static AString getDefaultIp();
  static AString getIPFromAddress(const AString &address);
  
  /*!
  Checks if port is available
  */
  static bool canBindToPort(int port, const AString& ip = AConstant::ASTRING_EMPTY);
  
  /*!
  Converting A.B.C.D to unsigned long byte version in network order DCBA and vice versa
  */
  static void convertIp4ToString(u4 ip, AOutputBuffer& );
  static u4 convertStringToIp4(const AString& ip);

  /*!
  Get MAC address associated with an IP
  */
  static AString ASocketLibrary::getMACfromIP(const AString& strIP);
};

#endif

/*
Usage example:

#include "AFile_Socket.hpp"

//When program exits, the destructor will deinitialize the library
ASocketLibrary g_ApplicationLibraryInstance;

int main()
{
  // Code here
}


*/