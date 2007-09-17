#ifndef INCLUDED__AFile_Socket_HPP__
#define INCLUDED__AFile_Socket_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "AFile.hpp"
#include "ASocketLibrary.hpp"

class ASocketListener;
class AHTTPRequestHeader;

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
  virtual void flush() {}  //a_Does not apply for sockets
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
  Reading of non-blocking sockets.
  */
  size_t readBlocking(void *, size_t size);
    
  /*!
  Check socket
  */
  bool isInputWaiting() const;
  bool isOutputBlocked() const;
  virtual bool isOpen();

  /*!
  Bytes avaialable (in socket and lookahead)
  */
  inline size_t getTotalBytesAvailableForRead() const;

  /*!
  Socket info
  */
  const ASocketLibrary::SocketInfo& getSocketInfo() const;

protected :
  //a_Implement raw read/write methods that AFile will use via socket interface
  virtual size_t _write(const void *, size_t);
  virtual size_t _read(void *, size_t);
  virtual bool _isNotEof();

  //a_Lookahead buffer used by read/write/peek overridden due to blocking socket read mode
  virtual size_t _readBlockIntoLookahead();

  //a_Physical bytes available in the socket (does not include the lookahead buffer)
  virtual size_t _availableInputWaiting() const;

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

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AFile_Socket_HPP__

