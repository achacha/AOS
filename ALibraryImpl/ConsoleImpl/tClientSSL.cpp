#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AFile_Socket_SSL.hpp"
#include "ASocketLibrary.hpp"

ASocketLibrary g_SocketLibrary;

int main()
{
  AHTTPRequestHeader request;
  request.useUrl().parse("https://www.openssl.org/");
  request.setPair(AHTTPHeader::HT_REQ_Host, "www.openssl.org");
  AHTTPResponseHeader response;

  try
  {
    AString& strIp = ASocketLibrary::getIPFromAddress(request.getUrl().getServer());
    AFile_Socket_SSL skt(strIp, request.useUrl().getPort());
    skt.open();
    request.toAFile(skt);
    response.fromAFile(skt);
    
    request.debugDump();
    response.debugDump();

    ARope rope;
    skt.readUntilEOF(rope);
    std::cout << rope << std::endl;
    skt.close();

    response.debugDump();
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown error" << std::endl;
  }

  return 0;
}
