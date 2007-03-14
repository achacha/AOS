#include "apiABase.hpp"
#include "apiASystem.hpp"
#include "apiAWeb.hpp"
#include "ASocketLibrary.hpp"
#include "AFile_Socket.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"

ASocketLibrary g_socketLibrary;

int main()
{
  AHTTPRequestHeader request;
  AHTTPResponseHeader response;

  //a_Build request
  request.useUrl().parse(ASWNL("http://127.0.0.1/ping"));

  try
  {
    AFile_Socket socket;
    socket.open("127.0.0.1", 12345, true);
    request.toAFile(socket);
    response.fromAFile(socket);

    std::cout << "Response status: " << response.getStatusCode() << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception" << std::endl;
  }

  return 0;
}
