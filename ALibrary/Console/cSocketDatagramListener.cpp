#include "AFile_SocketDatagram.hpp"

int main()
{
  AFile_SocketDatagram listener;

  listener.bind(6666);

  AString header, line;
  while (line.compare("<eot>"))
  {
    listener.receiveFrom();

    listener.readString(header, 6);
    listener.readLine(line);
    std::cout << header.c_str() << ":" << line.c_str() << std::endl;
  }
  
  return 0;
}
