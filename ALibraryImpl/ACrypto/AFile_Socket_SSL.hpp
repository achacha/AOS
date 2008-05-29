/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AFile_Socket_SSL_HPP__
#define INCLUDED__AFile_Socket_SSL_HPP__

#include "apiACrypto.hpp"
#include "AString.hpp"
#include "AFile_Socket.hpp"

class ASocketListener_SSL;

/*!
 * AFile implementation using SSL sockets
 * To create a socket you MUST have an instance of ASocketLibrary anywhere in your module.
 *  This is a global object that will initialize the socket library and deinitialize it when it goesout of scope
 *  For performance reasons it's better to keep one around for the duration of the time that sockets are used
**/
class ACRYPTO_API AFile_Socket_SSL : public AFile_Socket
{
public:
  // construction 
  AFile_Socket_SSL(const AString& address, int portNum);
  AFile_Socket_SSL(ASocketListener_SSL& listener);
  virtual ~AFile_Socket_SSL();

  virtual void open();
  virtual void flush() {}  //a_Does not apply for sockets
  virtual void close();

  size_t readBlocking(void *buf, size_t size);

  void throwSSLError(int sslErrorCode);

  virtual size_t readBlockIntoLookahead();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected :
  //a_Implement raw read/write methods that AFile will use via socket interface
  virtual size_t _write(const void *, size_t);
  virtual size_t _read(void *, size_t);

  virtual size_t _availableInputWaiting() const;

private:
  //a_Prevent empty sockets and socket object copy
  //AFile_Socket_SSL(const AFile_Socket_SSL &) {}
  AFile_Socket_SSL &operator=(const AFile_Socket_SSL &) { return *this; }

  void _initSSL(bool isOutboundSocket);
  void _connectSSL();
  void _deinitSSL();
  void *mp_SSLData;
};

#endif // INCLUDED__AFile_Socket_SSL_HPP__
