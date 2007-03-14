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

  if (m_Services.useConfiguration().getAosServerPort() > 0)
  {
    mthread_Listener.setThis(static_cast<void *>(this));
    mthread_Listener.start();
  }
  else
  {
    ALock lock(m_Services.useScreenSynch());
    std::cout << "HTTP disabled, not listening." << std::endl;
  }

  if (m_Services.useConfiguration().getAosSecureServerPort() > 0)
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
    ASocketListener listener(pThis->m_Services.useConfiguration().getAosServerPort());
    listener.open();

    pThis->m_Services.useLog().add(ASWNL("AObjectServer started.  HTTP listening on port ")+AString::fromInt(pThis->m_Services.useConfiguration().getAosServerPort()));
    {
      ALock lock(pThis->m_Services.useScreenSynch());
      std::cout << "AObjectServer HTTP listening on port " << pThis->m_Services.useConfiguration().getAosServerPort() << std::endl;
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

  thread.setRunning(true);

  ASocketListener_SSL listener(
    pThis->m_Services.useConfiguration().getAosSecureServerPort(),
    pThis->m_Services.useConfiguration().getAosCertFilename(),
    pThis->m_Services.useConfiguration().getAosPKeyFilename()
  );
  listener.open();

  pThis->m_Services.useLog().add(ASWNL("AObjectServer started.  HTTPS listening on port ")+AString::fromInt(pThis->m_Services.useConfiguration().getAosSecureServerPort()));
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTPS listening on port " << pThis->m_Services.useConfiguration().getAosSecureServerPort() << std::endl;
  }

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
