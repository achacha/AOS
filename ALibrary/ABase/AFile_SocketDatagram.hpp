#ifndef INCLUDED__AFile_SocketDatagram_HPP__
#define INCLUDED__AFile_SocketDatagram_HPP__

#include "apiABase.hpp"
#include "AFile_Socket.hpp"

/*!
Socket datagram
To create a socket you MUST have an instance of ASocketLibrary anywhere in your module.
This is a global object that will initialize the socket library and deinitialize it when it goesout of scope
For performance reasons it's better to keep one around for the duration of the time that sockets are used
*/

class ABASE_API AFile_SocketDatagram : public AFile
{
public:
  // construction 
  AFile_SocketDatagram();
  virtual ~AFile_SocketDatagram();

  /*!
    Bind address/port for listener only

    Sample usage:
      AFile_SocketDatagram listener;
      AFile_SocketDatagram sender;
      
      listener.bind(6666);
      
      sender.writeLine("Hello World!");
      sender.sendTo(6666, "127.0.0.1");

      listener.receiveFrom();

      AString line;
      listener.readLine(line);
      std::cout << line.c_str() << std::endl;
  */
  void bind(int port=0, const AString& ipAddr = ASocketLibrary::ANY_ADDRESS);

  virtual void open() {}     //a_Does not apply to datagrams
  virtual void flush() {}    //a_Does not apply to datagrams

  //a_Close the socket handle (in case explicit close is needed)
  virtual void close();

  /*!
    Send the data contained in the internal buffer
    Given maximum datagram size, this will split the data into packets
      this call may send out several packets to the destination if buffer > max datagram size
  */
  size_t sendTo(int port, const AString& address);
  
  /**
    Receive ONE packet of max datagram size or less
    Will block if no data is present and wait
  */
  size_t receiveFrom(int port = 0, const AString& address = ASocketLibrary::ANY_ADDRESS);

  //a_Check socket
  bool isInputWaiting() const;

  //a_Access the internal read/write buffer
  //a_Use the AFile based methods as usual, this is just a specialized way to get to the buffer
  AString& useBuffer() { return m_Buffer; }

  //a_Set/get maximum datagram size
  size_t getMaximumDatagramSize() const { return m_MaximumDatagramSize; }
  void setMaximumDatagramSize(size_t size) { m_MaximumDatagramSize = size; }

  //a_Datagram options for send and receive
  void setReceiveTimeout(size_t miliSec);
  void setSendTimeout(size_t miliSec);

protected:
	virtual size_t _read(void *, size_t);
	virtual size_t _write(const void *, size_t);
  virtual bool _isNotEof();

private:
  void _makeHandle();

  ASocketLibrary::SocketInfo m_SocketInfo;    //a_Contains socket information

  AString m_Buffer;

  size_t m_MaximumDatagramSize;
};

#endif // INCLUDED__AFile_SocketDatagram_HPP__
