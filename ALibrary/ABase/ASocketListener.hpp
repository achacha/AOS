#ifndef INCLUDED__ASocketListener_HPP__
#define INCLUDED__ASocketListener_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AFile_Socket.hpp"

/*
 * Socket listener
 * To create a socket you MUST have an instance of ASocketLibrary anywhere in your module.
 *  This is a global object that will initialize the socket library and deinitialize it when it goesout of scope
 *  For performance reasons it's better to keep one around for the duration of the time that sockets are used
**/
class ABASE_API ASocketListener : public ADebugDumpable
{
friend AFile_Socket;    //a_This class will use the accept method to connect itself

public:
  /*!
  ip - This is the actual IP address, to convert hostname to ip use call in ASocketLibrary
  allowLocalReuse - when true socket can be bound to an address/port already in use
  backlog - listener socket backlog queue
  if ip is empty, default interface is assumed 
  */
  ASocketListener(int port, const AString& ip = AConstant::ASTRING_EMPTY, int backlog = 42, bool allowLocalReuse = false);
  virtual ~ASocketListener();
  
  /*!
  Open listener, now use AFile_Socket to accept (see AFile_Socket for the call to use the listener)
  */
  void open();
  
  /*!
  Returns:
    true if there are pending accepts
    false if the next accept will block
  */
  bool isAcceptWaiting();

  /*!
  Close the listener
  */
  void close();

  /*!
  Read timeout
  */
  void setReadTimeout(s4 miliSec);
  
  /*!
  Write timeout
  */
  void setWriteTimeout(s4 miliSec);

  /*!
  SocketInfo
  */
  const ASocketLibrary::SocketInfo& getSocketInfo() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  ASocketLibrary::SocketInfo accept();

  bool m_AllowReuse;
  int m_backlog;

  void makeHandle();
  ASocketLibrary::SocketInfo m_SocketInfo;
};

#endif //INCLUDED__ASocketListener_HPP__
