#ifndef INCLUDED__ASocketListener_SSL_HPP__
#define INCLUDED__ASocketListener_SSL_HPP__

#include "apiACrypto.hpp"
#include "AFile_Socket_SSL.hpp"
#include "ASocketListener.hpp"

/*!
SSL socket listener
To create a socket you MUST have an instance of ASocketLibrary anywhere in your module.
 This is a global object that will initialize the socket library and deinitialize it when it goesout of scope
 For performance reasons it's better to keep one around for the duration of the time that sockets are used
*/
class ACRYPTO_API ASocketListener_SSL : public ASocketListener
{
friend AFile_Socket_SSL;    //a_This class will use the accept method to connect itself

public:
  /*!
  allowLocalReuse - when true socket can be bound to an address/port already in use
  */
  ASocketListener_SSL(int port, const AString& certFilename, const AString& keyFilename, const AString &ip = AConstant::ASTRING_EMPTY, int backlog = 5, bool allowLocalReuse = false);
  virtual ~ASocketListener_SSL();
  
  /*!
  Open listener, now use AFile_Socket to accept (see AFile_Socket for the call to use the listener)
  */
  void open();

  /*!
  Close the listener
  */
  void close();

  void throwSSLError(int sslErrorCode);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  ASocketLibrary::SocketInfo accept(void *pSSLData);

private:
  void *mp_SSLData;

  const AString m_certFilename;
  const AString m_keyFilename;

  //a_Server
  void _initSSL();

  //a_Client
  void _initClientSSL(void *pSSLData);
};

#endif //INCLUDED__ASocketListener_SSL_HPP__
