#include <AString.hpp>
#include <AFile_IOStream.hpp>
#include <AFile_Socket.hpp>
#include <AThread.hpp>
#include <ALock.hpp>
#include <ACriticalSection.hpp>
#include <AMutex.hpp>
#include <list>
#include <ASocketListener.hpp>
#include <ASocketException.hpp>
#include <AHTTPResponseHeader.hpp>

#define THREAD_FLAG_DEAD 0

AMutex g_cleanup("cleanThread");
std::list<AThread *> threads;

ASocketLibrary g_SocketLib;
ACriticalSection g_lockOutput;

u4 cleanupThreadProc(AThread& thread)
{
  while (true)
  {
    AThread::sleep(5000);
    {
      ALock lock(g_cleanup);
      std::list<AThread *>::iterator it = threads.begin();
      while (it != threads.end())
      {
        if ((*it)->useBitArray().isBit(THREAD_FLAG_DEAD))
        {
          std::list<AThread *>::iterator kill = it;
          ++it;
          AThread *pKill = (*kill);
          std::cout << "Removing thread: " << pKill->getId() << std::endl;
          threads.erase(kill);
          delete pKill;
        }
      }
    }
  }
}

u4 threadProc(AThread& thread)
{
  try
  {
    AFile_Socket& socket = *(AFile_Socket *)thread.getParameter();

    AString input;
    
    {
      ALock lock(g_lockOutput);
      std::cout << "===REQUEST {" << std::endl;
      while (AString::npos != socket.readLine(input))
      {
        std::cout << input << std::endl;
        input.clear();
      }
      std::cout << "} ===REQUEST" << std::endl;
    }

    //a_Now reply with all is well
    AHTTPResponseHeader response;
    response.setStatusCode(AHTTPResponseHeader::SC_Ok);
    response.toAFile(socket);
    socket.writeLine("\r\n<html>Ok</html>");
    socket.close();
    
    {
      ALock lock(g_lockOutput);
 	    std::cout << "Client exited, thread shutting down." << std::endl;
    }
    thread.useBitArray().setBit(THREAD_FLAG_DEAD);
  }
  catch(AException& e)
  {
    thread.useBitArray().setBit(THREAD_FLAG_DEAD);
    std::cout << e.what() << std::endl;
  }
  return 0;
}

int main(int, char **)
{
  AThread cleanupSocket(cleanupThreadProc, true);

  AString error;
  ASocketListener listener;
  try
  {
    listener.open(1366);
  }
  catch(const AException& e)
  {
    error = e.what();
  }
  std::cout << error.c_str() << std::endl;

  while (true)
  {
    {
      ALock lock(g_lockOutput);
    	std::cout << "Server started, listening on port 1366." << std::endl;
    }
    AFile_Socket *p = new AFile_Socket(listener);
    {
      ALock lock(g_lockOutput);
    	std::cout << "Connection received, worker thread spawned." << std::endl;
    }

    AThread *t = new AThread(threadProc);
    t->setParameter(p);
    t->start();
    {
      ALock lock(g_cleanup);

      {
        ALock lock(g_lockOutput);
        std::cout << "Adding thread: " << t->getId() << std::endl;
      }
      threads.push_back(t);
    }
  }

	return 0;
}
