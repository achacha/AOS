#include <AString.hpp>
#include <AFile_IOStream.hpp>
#include <AFile_Socket.hpp>
#include <AThread.hpp>
#include <ALock.hpp>
#include <AMutex.hpp>
#include <ASocketListener.hpp>
#include <ASocketException.hpp>
#include <AHttpRequestHeader.hpp>
#include <AHttpResponseHeader.hpp>
#include <AContentTypeFactory.hpp>
#include <AContentType_Form.hpp>
#include <templateAutoPtr.hpp>
#include <ARandomNumberGenerator.hpp>

#define THREAD_FLAG_DEAD 0

AMutex g_cleanup("cleanThread");
std::list<AThread *> threads;

ASocketLibrary g_SocketLib;

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

    //a_REQUEST
    AHTTPRequestHeader requestHeader;
    requestHeader.fromAFile(socket);
    AContentTypeInterface *pHttpDoc = AContentTypeFactory::createContentTypeDocument(requestHeader);
    if (pHttpDoc)
    {
      pHttpDoc->fromAFile(socket);
      AContentType_Form *pctForm = dynamic_cast<AContentType_Form *>(pHttpDoc);
      if (pctForm)
      {
        std::cout << "FORM:" << pctForm->useForm() << std::endl;
      }
      else
      {
        std::cout << "UNK: Content-Type: " << pHttpDoc->getContentType() << " and " << pHttpDoc->getContentLength() << std::endl;
      }
    }

    //a_RESPONSE
    AHTTPResponseHeader responseHeader;
    responseHeader.setPair(AHTTPHeader::HT_GEN_Connection, "close");
    responseHeader.toAFile(socket);
    socket.writeLine(AString::fromU4(ARandomNumberGenerator::get().nextU4()));
    socket.close();
    
    std::cout << "Client exited, thread " << thread.getId() << " shutting down." << std::endl;
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
  	std::cout << "\r\nServer started, listening on port 1366." << std::endl;
    AFile_Socket *p = new AFile_Socket(listener);
	  std::cout << "Connection received, worker thread spawned." << std::endl;

    AThread *t = new AThread(threadProc);
    t->setParameter(p);
    t->start();
    {
      ALock lock(g_cleanup);
      std::cout << "Adding thread: " << t->getId() << std::endl;
      threads.push_back(t);
    }
  }

	return 0;
}
