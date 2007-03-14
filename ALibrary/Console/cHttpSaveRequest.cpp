#include <AString.hpp>
#include <AFile_Socket.hpp>
#include <AThread.hpp>
#include <AException.hpp>
#include "AFile_Physical.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "ATimer.hpp"
#include "AMovingAverage.hpp"

ASocketLibrary g_ApplicationLibraryInstance;

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: this <url> <targetfile>\r\n";
    std::cout << "\r\nExample: this http://localhost:8000/myrequest.jsp savefile.txt" << std::endl;
    return -1;
  }
  
  AFile_Socket socket;

  //a_Parse URL
  AHTTPRequestHeader requestHdr;
  requestHdr.useUrl().parse(argv[1]);
  


  AFile_Physical savefile(argv[2], "w");
  
  AString hostAddr = g_ApplicationLibraryInstance.getIPFromAddress(requestHdr.useUrl().getServer());

  requestHdr.setMethod("GET");
  requestHdr.setPair("Host", hostAddr);
  requestHdr.setPair("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.6) Gecko/20040206 Firefox/0.8");
  requestHdr.setPair("Accept", "application/x-shockwave-flash,text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1");
  requestHdr.setPair("Accept-Language", "en-us,en;q=0.5");
  requestHdr.setPair("Accept-Encoding", "gzip,deflate");
  requestHdr.setPair("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
  requestHdr.setPair("Accept", "*/*");
  requestHdr.setPair("Connection", "close");

  AString data(2048, 1024);
  try
  {
    //a_Send request
    socket.open(hostAddr, requestHdr.useUrl().getPort(), true);
    requestHdr.emit(socket);

    //a_Read response
    char c = '\x0';
    while (socket.isNotEof())
    {
      if (0 == socket.read(c))
        AThread::sleep(20);
      else
        data.append(c);
    }
    socket.close();

    data.debugDump();

    savefile.write(data);
    savefile.close();
  }
  catch(AException& e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
