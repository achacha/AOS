#include <AFile_SocketDatagram.hpp>

int main()
{
  AFile_SocketDatagram listener;
  AFile_SocketDatagram sender;
  
  listener.bind(6666);
  
  sender.writeLine("Hello World!");
  sender.sendTo(6666, "127.0.0.1");

  listener.receiveFrom();

  AString line;
  listener.readLine(line);
  std::cout << line.c_str() << std::endl;

  return 0;
}
