#include "AString.hpp"
#include "AException.hpp"
#include "ASocket.hpp"
#include "ASocketNativeData.hpp"
#include "AThread.hpp"

int main()
{

/*
  ASocketNativeData data;
  data.useSOCKET() = 1;
  data.setSAIAddress(0x61626364);
  data.setSAIPort(666);
  cout << "SOCKET=" << data.useSOCKET() << endl;
  cout << "SOCKADDR_IN.sin_family=" << data.getSAIFamily() << endl;
  cout << "SOCKADDR_IN.sin_port=" << data.getSAIPort() << endl;
  cout << "SOCKADDR_IN.sin_addr=" << hex << AString::fromLong(data.getSAIAddress()).c_str() << endl;
  cout << "SOCKADDR_IN.sin_addrA=" << data.getSAIAddressA() << endl;
  cout << "SOCKADDR_IN.sin_addrB=" << data.getSAIAddressB() << endl;
  cout << "SOCKADDR_IN.sin_addrC=" << data.getSAIAddressC() << endl;
  cout << "SOCKADDR_IN.sin_addrD=" << data.getSAIAddressD() << endl;
*/

  
  ASocket sock("172.16.10.45", 80, true);
  int iState = sock.open();
  if (ASocket::WOULD_BLOCK == iState)
  {
    int iCount = 0x0;
    while (!sock.canWrite() && iCount++ < 10)
    {
      cerr << "{W-BLOCK}";
      AThread::sleep(25);
    }

    //a_No timeout, socket ready for write
    if (iCount < 10)
      iState = 0x0;
  }

  if (0x0 == iState)
  {
    AString strBuffer("GET / HTTP/1.0\r\n\r\n");
    sock.write(strBuffer.c_str(), strBuffer.length());
  }

  try
  {
    AString strBuffer(128, 16);
    int iRead = -1;
    while (iRead)
    {
      iRead = sock.read((void *)strBuffer.getCharPtr(), strBuffer.getSize() - 0x1);
      if (ASocket::WOULD_BLOCK == iRead)
      {
        int iCount = 0x0;
        while (!sock.canWrite() && iCount++ < 10)
        {
          cerr << "{R-BLOCK}";
          AThread::sleep(25);
        }
        //a_No timeout, socket ready for write
        if (iCount >= 10)
          break;
      }
      if (iRead > 0x0)
      {
        strBuffer[iRead] = '\x0';
        cout << strBuffer << flush;
      }
    }
  }
  catch(AException& e)
  {
    cerr << e.what() << endl;
  }
  catch(...)
  {
    cerr << "Unknown exception caught." << endl;
  }

  return 0x0;
}
