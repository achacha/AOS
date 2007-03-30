#include "pchAOS_Base.hpp"
#include "AOSRequestListener.hpp"
#include "ASocketListener.hpp"
#include "ASocketListener_SSL.hpp"
#include "AFile_Socket_SSL.hpp"
#include "ASocketException.hpp"
#include "AOSServices.hpp"
#include "ALock.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSConfiguration.hpp"

#define WAIT_FOR_CONNECTION_DELAY 100

const AString& AOSRequestListener::getClass() const
{
  static const AString CLASS("AOSRequestListener");
  return CLASS;
}

void AOSRequestListener::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

  addProperty(eBase, ASW("listener_running",16), (mthread_Listener.isRunning() ? AString::sstr_True : AString::sstr_False));
  addProperty(eBase, ASW("secure_listener_running",23), (mthread_SecureListener.isRunning() ? AString::sstr_True : AString::sstr_False));
  addProperty(eBase, ASW("listener_run",12), (mthread_Listener.isRun() ? AString::sstr_True : AString::sstr_False));
  addProperty(eBase, ASW("secure_listener_run",19), (mthread_Listener.isRun() ? AString::sstr_True : AString::sstr_False));
}

AOSRequestListener::AOSRequestListener(
  AOSServices& services,
  AOSContextQueueInterface *pFirstQueue
) :
  mthread_Listener(AOSRequestListener::threadprocListener),
  mthread_SecureListener(AOSRequestListener::threadprocSecureListener),
  m_Services(services),
  mp_FirstQueue(pFirstQueue)
{
  AASSERT(this, mp_FirstQueue);

  registerAdminObject(m_Services.useAdminRegistry());
}

AOSRequestListener::~AOSRequestListener()
{
}

void AOSRequestListener::startListening()
{
  if (mthread_Listener.isThreadActive() || mthread_SecureListener.isThreadActive())
    ATHROW(this, AException::ProgrammingError);

  int http_port = m_Services.useConfiguration().getIntWithDefault(AOSConfiguration::LISTEN_HTTP_PORT, -1);
  if (http_port > 0)
  {
    mthread_Listener.setThis(static_cast<void *>(this));
    mthread_Listener.start();
  }
  else
  {
    ALock lock(m_Services.useScreenSynch());
    std::cout << "HTTP disabled, not listening." << std::endl;
  }

  int https_port = m_Services.useConfiguration().getIntWithDefault(AOSConfiguration::LISTEN_HTTPS_PORT, -1);
  if (https_port > 0)
  {
    mthread_SecureListener.setThis(static_cast<void *>(this));
    mthread_SecureListener.start();
  }
  else
  {
    ALock lock(m_Services.useScreenSynch());
    std::cout << "HTTPS disabled, not listening." << std::endl;
  }
}

void AOSRequestListener::stopListening()
{
  mthread_Listener.setRun(false);
  mthread_SecureListener.setRun(false);
}

u4 AOSRequestListener::threadprocListener(AThread& thread)
{
  AOSRequestListener *pThis = static_cast<AOSRequestListener *>(thread.getThis());
  AASSERT(NULL, pThis);

  thread.setRunning(true);
  AOSContext *pContext = NULL;
  try
  {
    int http_port = pThis->m_Services.useConfiguration().getInt(AOSConfiguration::LISTEN_HTTP_PORT);
    ASocketListener listener(http_port);
    listener.open();

    pThis->m_Services.useLog().add(ASWNL("AObjectServer started.  HTTP listening on port ")+AString::fromInt(http_port));
    {
      ALock lock(pThis->m_Services.useScreenSynch());
      std::cout << "AObjectServer HTTP listening on port " << http_port << std::endl;
    }

    while(thread.isRun())
    {
      if (listener.isAcceptWaiting())
      {
        AAutoPtr<AFile_Socket> pSocket(NULL);
        try
        {
          pSocket.reset(new AFile_Socket(listener, true));
          pSocket->open();
          pContext = pThis->m_Services.useContextManager().allocate(pSocket.get());
          pSocket.setOwnership(false);
        }
        catch(ASocketException& e)
        {
          pThis->m_Services.useLog().add(e);
          
          pSocket->close();
          if (pContext)
            pThis->m_Services.useContextManager().deallocate(pContext);

          pContext = NULL;
          pSocket.reset();

          continue;
        }
        catch(...)
        {
          pThis->m_Services.useContextManager().deallocate(pContext);

          pContext = NULL;
          pSocket.reset();

          throw;
        }
        
        if (pContext)
        {
          pThis->mp_FirstQueue->add(pContext);
        }
        pSocket.reset();
      }
      else
        AThread::sleep(WAIT_FOR_CONNECTION_DELAY);
    }
  }
  catch(AException& e)
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTP listener thread caught an exception: " << e.what() << std::endl;
    pThis->m_Services.useLog().add(e);
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
    throw;
  }
  catch(...)
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTP listener thread caught an unknown exception." << std::endl;
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
    throw;
  }

  thread.setRunning(false);
  return 0;
}

u4 AOSRequestListener::threadprocSecureListener(AThread& thread)
{
  AOSRequestListener *pThis = static_cast<AOSRequestListener *>(thread.getThis());
  AASSERT(NULL, pThis);


  int https_port = pThis->m_Services.useConfiguration().getInt(AOSConfiguration::LISTEN_HTTPS_PORT);
  AString cert, pkey;
  if (!pThis->m_Services.useConfiguration().getString(ASW("/config/base/crypto/cert",24), cert))
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTPS missing /config/base/crypto/cert, secure socket not listening." << std::endl;
    return -1;
  }
  if (!pThis->m_Services.useConfiguration().getString(ASW("/config/base/crypto/pkey",24), pkey))
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTPS missing /config/base/crypto/pkey, secure socket not listening." << std::endl;
    return -1;
  }

  ASocketListener_SSL listener(https_port, cert, pkey);
  listener.open();

  pThis->m_Services.useLog().add(ASWNL("AObjectServer started.  HTTPS listening on port ")+AString::fromInt(https_port));
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTPS listening on port " << https_port << std::endl;
  }

  thread.setRunning(true);
  AOSContext *pContext = NULL;
  try
  {
    while(thread.isRun())
    {
      if (listener.isAcceptWaiting())
      {
        AAutoPtr<AFile_Socket> pSocket(NULL);
        try
        {
          pSocket.reset(new AFile_Socket_SSL(listener));
          pSocket->open();
          pContext = pThis->m_Services.useContextManager().allocate(pSocket.get());
          pSocket.setOwnership(false);
          pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_HTTPS);
        }
        catch(AException& e)
        {
          pThis->m_Services.useLog().add(e);

          pSocket->close();
          if (pContext)
            pThis->m_Services.useContextManager().deallocate(pContext);

          pContext = NULL;
          pSocket.reset();

          continue;
        }
        catch(...)
        {
          pThis->m_Services.useContextManager().deallocate(pContext);
          pContext = NULL;
          pSocket.reset();

          throw;
        }
        
        if (pContext)
        {
          pThis->mp_FirstQueue->add(pContext);
        }
        pSocket.reset();
      }
      else
        AThread::sleep(WAIT_FOR_CONNECTION_DELAY);
    }
  }
  catch(AException& e)
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTPS listener thread caught an exception: " << e.what() << std::endl;
    pThis->m_Services.useLog().add(e);
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
    throw;
  }
  catch(...)
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer listener thread caught an unknown exception." << std::endl;
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
    throw;
  }

  thread.setRunning(false);
  return 0;
}

bool AOSRequestListener::isRunning() const
{
  return (mthread_Listener.isRunning() && mthread_SecureListener.isRunning());
}
