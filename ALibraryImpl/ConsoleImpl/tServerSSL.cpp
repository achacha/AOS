#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "ASocketListener_SSL.hpp"
#include "AFile_Socket_SSL.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "AFile_IOStream.hpp"

ASocketLibrary_SSL g_SecureSocketLibrary;

int main()
{
  ASocketListener_SSL sslListener(443, ASWNL("aosconfig/aos_cert.pem"), ASWNL("aosconfig/aos_pkey.pem"));
  AFile_Socket_SSL skt(sslListener);
  try
  {
    std::cout << "Listening on port 443..." << std::endl;
    sslListener.open();
    skt.open();

    AHTTPResponseHeader response;
    AHTTPRequestHeader request;
    request.fromAFile(skt);

    ARope rope;
    skt.readUntilEOF(rope);
    std::cout << rope << std::endl;

    //a_Display
    AFile_IOStream io;
    request.emit(io);
    io.writeLine(AString::sstr_Empty);
    rope.emit(io);
    io.writeLine(AString::sstr_Empty);
    io.writeLine(AString::sstr_Empty);
    response.emit(io);

    //a_Response
    response.setPair(AHTTPHeader::HT_ENT_Content_Type, "text/plain");

    response.toAFile(skt);
    request.debugDumpToAOutputBuffer(skt);
    skt.writeLine(AString::sstr_Empty);
    rope.emit(skt);
    skt.writeLine(AString::sstr_Empty);
    skt.writeLine(AString::sstr_Empty);
    response.debugDumpToAOutputBuffer(skt);

    skt.close();
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
