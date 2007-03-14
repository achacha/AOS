#include "AFile_SocketDatagram.hpp"
#include "AFile_IOStream.hpp"

int main(int argc, char **argv)
{
  
  AFile_SocketDatagram sender;
  AFile_IOStream io;

  AString header("DATA_0"), line;
  while (line.compare("<eot>"))
  {
    std::cout << "> " << std::flush;
    io.readLine(line);

    //a_Double send to test
    sender.writeString(header);
    sender.writeLine(line);
    sender.sendTo(6666, "127.0.0.1");
  }
  
  return 0;
}
