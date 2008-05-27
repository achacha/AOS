/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AFile_Socket_HPP__
#define INCLUDED__AFile_Socket_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "AFile.hpp"
#include "ASocketLibrary.hpp"

class ASocketListener;
class AHTTPRequestHeader;
class AUrl;

/*!
 * AFile implementation using sockets
 * To create a socket you MUST have an instance of ASocketLibrary anywhere in your module.
 *  This is a global object that will initialize the socket library and deinitialize it when it goesout of scope
 *  For performance reasons it's better to keep one around for the duration of the time that sockets are used
**/
class ABASE_API AFile_Socket : public AFile
{
public:
  /*!
  ctor
  ip - actual string version of the IP, use ASocketLibrary utility functions to do lookup
  portNum - port to connect to
  blocking - if true, it will create a blocking socket
  */
  AFile_Socket(const AString& ip, int portNum, bool blocking = false);
  
  /*!
  ctor
  request - HTTP request object where address and port are contained, the ctor will do lookup to convert address to ip
  blocking - if true, it will create a blocking socket
  */
  AFile_Socket(const AHTTPRequestHeader& request, bool blocking = false);

  /*!
  ctor
  url - url object where address and port are contained, the ctor will do lookup to convert address to ip
  blocking - if true, it will create a blocking socket
  */
  AFile_Socket(const AUrl& url, bool blocking = false);

  /*!
  ctor
  listener - create a new socket based on a listener which has accepted a connection (inbound connection)
  blocking - if true, it will create a blocking socket
  */
  AFile_Socket(ASocketListener& listener, bool blocking = false);

  /*! 
  dtor 
  will close socket and release handle
  */
  virtual ~AFile_Socket();

  virtual void open();
  virtual void flush();
  //! Rewind does nothing for sockets
  virtual void rewind() {}
  virtual void close();

  /*!
  Timeout options on reads
  If using Blocking mode it may be necessary to set these to prevent waiting forever
  */
  void setReadTimeout(size_t miliSec);
  void setWriteTimeout(size_t miliSec);
  void setCloseTimeout(size_t miliSec);

  /*!
  Change to non-blocking mode
  */
  void setNonBlocking();
  void setBlocking();

  /*!
  Set Nagle algorithm for send coalescing
  When false, the data to be sent is not buffered and instead sent right away
    this will affect efficiency but useful when writing socket based console apps
  
  Default is true (when socket is created)
  */
  void setNagleAlgorithm(bool mode = true);
    
  /*!
  Check socket
  */
  bool isInputWaiting() const;
  bool isOutputBlocked() const;
  virtual bool isOpen();

  /*!
  Bytes avaialable (in socket and lookahead)
  */
  size_t getTotalBytesAvailableForRead() const;

  /*!
  Lookahead buffer used by read/write/peek overridden due to blocking socket read mode
  */
//  virtual size_t readBlockIntoLookahead();

  /*!
  Socket info
  */
  const ASocketLibrary::SocketInfo& getSocketInfo() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected :
  /*!
  Write using socket interface
  @return bytes written or if error exception thrown
  */
  virtual size_t _write(const void *, size_t);
  /*!
  Read using socket interface
  @return bytes read, if no data then AConstant::unavail if non-blocking
  */
  virtual size_t _read(void *, size_t);
  
  //! Checks if not eof
  virtual bool _isNotEof();

  //! Physical bytes available in the socket (does not include the lookahead buffer)
  virtual size_t _availableInputWaiting() const;

  size_t _readBlocking(void *buf, size_t size);
  size_t _readNonBlocking(void *buf, size_t size);
  size_t _writeBlocking(const void *buf, size_t size);
  size_t _writeNonBlocking(const void *buf, size_t size);

  void makeHandle();
  bool mbool_Blocking;           //a_Determines if blocking read needs to be used
  bool m_EOF;                    //a_EOF condition detected
  ASocketListener *mp_Listener;  //a_Non-NULL if listener is to be used

  ASocketLibrary::SocketInfo m_SocketInfo;    //a_Contains socket information

private:
  //a_Prevent empty sockets and socket object copy
  AFile_Socket() {}
  AFile_Socket(const AFile_Socket &) {}
  AFile_Socket &operator=(const AFile_Socket &) { return *this; }
};

#endif // INCLUDED__AFile_Socket_HPP__

