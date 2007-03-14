#include <AString.hpp>
#include <AFile_IOStream.hpp>
#include <AFile_Socket.hpp>
#include <AThread.hpp>
#include <ALock.hpp>
#include <AMutex.hpp>
#include <list>
#include <AException.hpp>
#include <templateAutoPtr.hpp>
#include "AFile_Physical.hpp"
#include "AHTTPRequestHeader.hpp"

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    std::cout << "Usage: this <API command> <API url> <Xml request>\n";
    std::cout << "Output: <xml request>.out.xml\n";
    std::cout << "\nExample: this http://localhost:8000/ws/api.dll myapirequest.xml\n  Result stored in: myapirequest.xml.out.xml\n" << std::endl;
    return -1;
  }
  
  AAutoPtr<AFile_Socket> socket(new AFile_Socket());
  AString strCommand(argv[1]);
  AString fileOut(argv[3]);
  fileOut += ".out.xml";

  //a_Parse URL
  AHTTPRequestHeader requestHdr;
  requestHdr.useUrl().parse(argv[2]);
  
  AString strXmlRequest;
  AFile_Physical file;
  if (!file.open(argv[3]))
  {
    std::cout << "Unable to open xml request: " << argv[3] << std::endl;
    return -2;
  }
  else
  {
    file.readUntilEOF(strXmlRequest);
    file.close();
    strXmlRequest.stripTrailing("\r\n ");
  }

  AString hostAddr = ASocketLibrary::getIPFromAddress(requestHdr.useUrl().getServer());
  try
  {
    socket->open(hostAddr, requestHdr.useUrl().getPort());
  }
  catch(AException& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }

  requestHdr.setMethod("POST");
  requestHdr.setPair("Host", hostAddr);
  requestHdr.setPair("X-EBAY-API-SESSION-CERTIFICATE", "Admintest1;AdminApp;AdminCertificate");
  requestHdr.setPair("X-EBAY-API-DEV-NAME", "Admintest1");
  requestHdr.setPair("X-EBAY-API-APP-NAME", "AdminApp");
  requestHdr.setPair("X-EBAY-API-CERT-NAME", "AdminCertificate");
  requestHdr.setPair("X-EBAY-API-SITEID", "0");
  requestHdr.setPair("X-EBAY-API-DETAIL-LEVEL", "14");
  requestHdr.setPair("X-EBAY-API-CALL-NAME", strCommand);
  requestHdr.setPair("X-EBAY-API-COMPATIBILITY-LEVEL", "305");
  requestHdr.setPair("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.6) Gecko/20040206 Firefox/0.8");
  requestHdr.setPair("Accept", "application/x-shockwave-flash,text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1");
  requestHdr.setPair("Accept-Language", "en-us,en;q=0.5");
  requestHdr.setPair("Accept-Encoding", "gzip,deflate");
  requestHdr.setPair("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
  requestHdr.setPair("Accept", "*/*");
  requestHdr.setPair("Connection", "close");
  requestHdr.setPair("Content-Type", "text/xml");
  requestHdr.setPair("Content-Length", AString::fromU4(strXmlRequest.getSize()));

  AString buffer;
  try
  {
    std::cout << "REQUEST\n--------" << std::endl;
    socket->writeString(requestHdr.toString());
    std::cout << requestHdr.toString() << std::flush;
    socket->writeString(strXmlRequest);
    std::cout << strXmlRequest << std::endl;

    std::cout << "\nRESPONSE\n---------" << std::endl;

    file.open(fileOut, "w");
    u4 bytesRead = socket->readUntilEOF(buffer);
    socket->close();

    std::cout << buffer << std::endl;
    file.writeString(buffer);
    file.close();
  }
  catch(AException& e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
