#include "AThread.hpp"
#include "AEvent.hpp"
#include "ALock.hpp"
#include "AFile_IOStream.hpp"

AEvent g_Event("myEvent", 10000);

u4 threadproc(AThread& thread)
{
  std::cout << "Starting event" << std::endl;

  {
    ALock lock(g_Event);
    std::cout << "\r\nSignaled event 1" << std::endl;
  }

  {
    ALock lock(g_Event);
    std::cout << "\r\nSignaled event 2" << std::endl;
  }

  std::cout << "Exiting thread" << std::endl;
  return 0;
}

int main()
{
  AThread thread(threadproc, true);
  AFile_IOStream io;
  AString input;

  AThread::sleep(10);
  std::cout << "press A + enter to signal event (or 'exit'): " << std::flush;
  while(AConstant::npos != io.readLine(input))
  {
    if (input.equals("exit"))
      break;

    g_Event.unlock();

    std::cout << "press A + enter to signal event (or 'exit'): " << std::flush;
    input.clear();
  }
  return 0;
}