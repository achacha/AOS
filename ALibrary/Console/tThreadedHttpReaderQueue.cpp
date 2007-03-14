#include "AThreadPool.hpp"
#include "ASocketListener.hpp"
#include "AFile_Socket.hpp"
#include "ACriticalSection.hpp"
#include "ALock.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "ALog_AFile.hpp"
#include "AFile_IOStream.hpp"
#include "AMovingAverage.hpp"

ASocketLibrary g_SocketLibrary;
ALog_AFile alog(new ACriticalSection(), AFilename(ASWNL("threaded_queue.log")));

class AQueueItem
{
public:
  AQueueItem(AFile_Socket *pSocket) : mp_Socket(pSocket), m_Timer(true) { }

  void reset()
  {
    try
    {
      m_RequestHeader.clear();
      pDelete(mp_Socket);
    }
    catch(...) {}
  }

  AFile_Socket *mp_Socket;
  ARopeDeque m_Buffer;
  AHTTPRequestHeader m_RequestHeader;
  ATimer m_Timer;

  static ACriticalSection m_StatisticsSynch;
  static AMovingAverage m_Average;
  static u4 m_ItemsAccepted;
  static u4 m_ItemsTimedOut;
  static u4 m_ItemsException;
  static u4 m_ItemsFinished;
};

u4 AQueueItem::m_ItemsAccepted = 0;
u4 AQueueItem::m_ItemsTimedOut = 0;
u4 AQueueItem::m_ItemsException = 0;
u4 AQueueItem::m_ItemsFinished = 0;
AMovingAverage AQueueItem::m_Average;
ACriticalSection AQueueItem::m_StatisticsSynch;

class AQueueChain : public AThreadPool
{
public:
  AQueueChain(ALog& alog, int workers = 1) :
    AThreadPool(threadproc, workers),
    m_Log(alog),
    mp_IfYesQueue(NULL), 
    mp_IfNoQueue(NULL)
  {
  }
  virtual ~AQueueChain() {}

  void setIfYes(AQueueChain *pQueue) { mp_IfYesQueue = pQueue; }
  void setIfNo(AQueueChain *pQueue)  { mp_IfNoQueue  = pQueue; }

  void addItem(AQueueItem *pItem)
  {
    ALock lock(m_QueueSynch);
    m_Queue.push_back(pItem);
  }

  AQueueItem *nextItem()
  {
    ALock lock(m_QueueSynch);
    AQueueItem *pItem = NULL;
    if (!m_Queue.empty())
    {
      pItem = m_Queue.front();
      m_Queue.pop_front();
    }
    return pItem;
  }

  virtual void process() = 0;

protected:
  typedef std::list<AQueueItem *> ITEMS;
  ITEMS m_Queue;
  ACriticalSection m_QueueSynch;

  AQueueChain *mp_IfYesQueue;
  AQueueChain *mp_IfNoQueue;

  ALog& m_Log;

  static u4 threadproc(AThread& thread);
};

u4 AQueueChain::threadproc(AThread& thread)
{
  AQueueChain *pThis = static_cast<AQueueChain *>(thread.getThis());
  AASSERT(NULL, pThis);
  thread.setRunning(true);
  while(thread.isRun())
  {
    try
    {
      if (!pThis->m_Queue.empty())
        pThis->process();
      else
        AThread::sleep(1);
    }
    catch(AException& ex)
    {
      pThis->m_Log.add(ex);
    }
    catch(...)
    {
      pThis->m_Log.add(ASWNL("Unknown Exception caught"));
      break;
    }
  }
  thread.setRunning(false);
  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
class QIsAvailable : public AQueueChain
{
public:
  QIsAvailable(ALog& alog, int workers = 1) :
    AQueueChain(alog, workers)
  {
  }

  virtual void process()
  {
    timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;
    fd_set sockSet;
    FD_ZERO(&sockSet);
    UINT count = 0;

    ITEMS::iterator it = m_Queue.begin();
    while(it != m_Queue.end() && count < FD_SETSIZE)
    {
      if ((*it)->m_Timer.getInterval() < 10000)
      {
        FD_SET((SOCKET)(*it)->mp_Socket->getSocketHandle(), &sockSet);
        ++it;
        ++count;
      }
      else
      {
        //a_Process timeout
        AQueueItem *pItem = *it;
        ITEMS::iterator itKill = it;
        ++it;
        
        {
          ALock lock(m_QueueSynch);
          m_Queue.erase(itKill);
        }
        delete pItem;
        {
          ALock lock(AQueueItem::m_StatisticsSynch);
          ++AQueueItem::m_ItemsTimedOut;
        }
      }
    }
    int ret = select(0, &sockSet, NULL, NULL, &timeout);
    if (SOCKET_ERROR == ret)
    {
      alog.add(ASWNL("Unexpected SOCKET_ERROR in select call in QIsAvailable"), ALog::FAILURE);
      return;
    }

    if (ret > 0)
    {
      count = 0;
      it = m_Queue.begin();
      while(it != m_Queue.end() && count < sockSet.fd_count)
      {
        if (FD_ISSET((*it)->mp_Socket->getSocketHandle(), &sockSet))
        {
          mp_IfYesQueue->addItem(*it);
          ITEMS::iterator itKill = it;
          ++it;
          ++count;
          
          {
            ALock lock(m_QueueSynch);
            m_Queue.erase(itKill);
          }
        }
      }
    }
  }
};
//////////////////////////////////////////////////////////////////////////////////////
class QReader : public AQueueChain
{
public:
  QReader(ALog& alog, int workers = 1) :
    AQueueChain(alog, workers)
  {
  }

  virtual void process()
  {
    AQueueItem *pItem = NULL;
    AString str(2048, 256);
    if (pItem = nextItem())
    {
      str.clear();
      size_t ret = pItem->mp_Socket->read(str, 2048);
      pItem->m_Buffer.append(str);

      str.clear();
      if (AConstant::npos != pItem->m_Buffer.popFrontUntil(str, ASW("\r\n\r\n", 4)))
      {
        //a_Have header
        pItem->m_RequestHeader.parse(str);
        pItem->mp_Socket->putBack(pItem->m_Buffer);
        mp_IfYesQueue->addItem(pItem);
      }
      else
      {
        //a_Header not complete
        mp_IfNoQueue->addItem(pItem);
      }
    }
  }
};
//////////////////////////////////////////////////////////////////////////////////////
class QHttpWriter : public AQueueChain
{
public:
  QHttpWriter(ALog& alog, int workers = 1) :
    AQueueChain(alog, workers)
  {
  }

  virtual void process()
  {
    AQueueItem *pItem = NULL;
    if (pItem = nextItem())
    {
      try
      {
        AHTTPResponseHeader responseHeader;
        responseHeader.setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html",9));

        double tc = pItem->m_Timer.getInterval();
        ARope output;
        responseHeader.emit(output);
        output.append("<html><body><pre>");
        pItem->m_RequestHeader.emit(output);
        output.append("\r\n<hr/>\r\n");
        responseHeader.emit(output);
        output.append("\r\n<hr/>\r\nContext timer=");
        output.append(AString::fromDouble(tc));
        output.append("\r\nItems accepted=");
        output.append(AString::fromU4(AQueueItem::m_ItemsAccepted));
        output.append("\r\nItems timed out=");
        output.append(AString::fromU4(AQueueItem::m_ItemsTimedOut));
        output.append("\r\nItems exceptioned=");
        output.append(AString::fromU4(AQueueItem::m_ItemsException));
        output.append("\r\nItems finished=");
        {
          ALock lock(AQueueItem::m_StatisticsSynch);
          AQueueItem::m_Average.addSample(tc);
          output.append(AString::fromU4(++AQueueItem::m_ItemsFinished));
          output.append("\r\nAverage=");
          AQueueItem::m_Average.emit(output);
          output.append("\r\nSamples=");
          output.append(AString::fromU8(AQueueItem::m_Average.getCount()));
        }
        output.append("</pre></body></html>");

        pItem->mp_Socket->write(output);
        
        pItem->mp_Socket->close();
        delete pItem;
      }
      catch(AException& ex)
      {
        alog.add(ex);
        {
          ALock lock(AQueueItem::m_StatisticsSynch);
          ++AQueueItem::m_ItemsException;
        }
        delete pItem;
      }
      catch(...)
      {
        alog.add(ASWNL("Unknown exception caught in QHttpWritter"));
        {
          ALock lock(AQueueItem::m_StatisticsSynch);
          ++AQueueItem::m_ItemsException;
        }
        delete pItem;
      }
    }
  }
};
//////////////////////////////////////////////////////////////////////////////////////
struct threadData
{
  int port;
};

u4 threadprocListener(AThread& thread)
{
  AQueueChain *pInQueue = (AQueueChain *)thread.getThis();
  threadData *pData = (threadData *)thread.getParameter();
  AASSERT(NULL, pInQueue);
  AASSERT(NULL, pData->port > 0);
  
  ASocketListener listener(pData->port, AString::sstr_Empty, SOMAXCONN, true);
  try
  {
    listener.open();
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
    return -1;
  }
  catch(...)
  {
    std::cerr << "Unknown exception caught." << std::endl;
    return -1;
  }

  thread.setRunning(true);
  std::cout << "Starting to listener #" << " on port " << pData->port << std::endl;
  while(thread.isRun())
  {
    AQueueItem *pItem = NULL;
    AFile_Socket *pSocket = NULL;
    try
    {
      pSocket = new AFile_Socket(listener, false);
      pSocket->open();
      pItem = new AQueueItem(pSocket);
      pInQueue->addItem(pItem);
      ++AQueueItem::m_ItemsAccepted;
    }
    catch(AException& e)
    {
      alog.add(e);
      pDelete(pItem);
      delete pSocket;
      {
        ALock lock(AQueueItem::m_StatisticsSynch);
        ++AQueueItem::m_ItemsException;
      }
      continue;
    }
    catch(...)
    {
      delete pItem;
      delete pSocket;
      {
        ALock lock(AQueueItem::m_StatisticsSynch);
        ++AQueueItem::m_ItemsException;
      }
      break;
    }
  }
  std::cout << "Listener stopped." << std::endl;
  thread.setRunning(false);
  return 0;
}

void realMain()
{
  //a_Create queues
  QIsAvailable  queueIsAvailable(alog, 1);
  QReader       queueReader(alog, 3);
  QHttpWriter   queueWriter(alog, 3);

  //a_Chain them
  queueIsAvailable.setIfYes(&queueReader);

  queueReader.setIfYes(&queueWriter);
  queueReader.setIfNo(&queueIsAvailable);

  //a_Start queues
  queueWriter.start();
  queueReader.start();
  queueIsAvailable.start();

  //a_Start listener
  threadData td0;
  td0.port = 7080;
  AThread threadListener(threadprocListener, true, &queueReader, (void *)&td0);

  AFile_IOStream io;
  AString str;
  while (true)
  {
    io.readLine(str);
    if (str.equalsNoCase("exit"))
      break;
    str.clear();

    AThread::sleep(1000);
  }
  threadListener.setRun(false);
  queueIsAvailable.stop();
  queueReader.stop();
  queueWriter.stop();
}

int main()
{
  try
  {
    realMain();
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception caught." << std::endl;
  }

  return 0;
}
