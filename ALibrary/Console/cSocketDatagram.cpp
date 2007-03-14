#include <AFile_SocketDatagram.hpp>
#include <AThread.hpp>
#include <AFile_IOStream.hpp>
#include <AMutex.hpp>
#include <ALock.hpp>

void globalLineOutput(const AString& line, bool boolDoCRLF = true)
{
  AMutex om("console_output");
  ALock lock(om);

  std::cout << line.c_str();
  if (boolDoCRLF)
    std::cout << std::endl;
  else
    std::cout << std::flush;
}

u4 threadprocListener(AThread& thread)
{
  globalLineOutput("RECV: Datagram listener on localhost:6666");
 
  try
  {
    AFile_SocketDatagram listener;
    listener.bind(6666);
 
    AString data;
    while (listener.receive() >= 0)
    {
      listener.readLine(data);
      globalLineOutput(AString("RECV: ")+data.c_str());
    }

  }
  catch(AException& ex)
  {
    std::cout << ex.what() << std::endl;
    return -1;
  }

  return 0x0;
}

int main()
{
  AFile_SocketDatagram sender;
  AThread thread(threadprocListener, true);

  AString input;
  AFile_IOStream io;
  while(input.compare("exit"))
  {
    globalLineOutput("'exit' >", false);
    
    io.readLine(input);
    sender.writeLine(input);
    sender.send(6666, "127.0.0.1");
  }
  
  return 0;
}

