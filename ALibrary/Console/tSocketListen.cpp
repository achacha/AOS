#include "AString.hpp"
#include "AException.hpp"
#include "ASocket.hpp"
#include "ASocketNativeData.hpp"
#include "AThread.hpp"

int main()
{
 
  ASocket sock("172.16.10.45", 81);

  try
  {
    sock.socketListen(5);
    AutoPtr<ASocket> pSock = sock.socketAccept();

    AString str(1024, 16);
    int iRead;
    while (iRead = pSock->read(str.getCharPtr(), str.getSize()))
    {
      str[iRead] = '\x0';
      cerr << str << flush;
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
