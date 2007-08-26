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

  int http_port = m_Services.useConfiguration().getInt(ASWNL("/config/server/listen/http"), -1);
  if (http_port > 0)
  {
    if (!ASocketLibrary::canBindToPort(http_port))
    {
      AString str("Unable to bind to http port ");
      str.append(AString::fromInt(http_port));
      str.append(", already in use.");
      m_Services.useConfiguration().emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
    }
    else
    {
      LISTEN_DATA *p = new LISTEN_DATA();
      p->port = http_port;
      p->pListener = this;

      mthread_Listener.setParameter(p);
      mthread_Listener.start();
    }
  }
  else
  {
    AOS_DEBUGTRACE("HTTP disabled, not listening.", NULL);
  }

  int https_port = m_Services.useConfiguration().getInt(ASWNL("/config/server/listen/https"), -1);
  if (https_port > 0)
  {
    AXmlNode *pNode = m_Services.useConfiguration().useConfigRoot().findNode(ASW("/config/server/crypto",21));
    if (!pNode)
    {
      AOS_DEBUGTRACE("AObjectServer HTTPS element not found, secure socket not listening.", NULL);
    }
    else
    {
      LISTEN_DATA *p = new LISTEN_DATA();
      p->port = https_port;
      p->pListener = this;

      pNode->emitFromPath(ASW("crypto/cert",11), p->cert); 
      pNode->emitFromPath(ASW("crypto/pkey",11), p->pkey);

      bool ready = true;
      if (p->cert.isEmpty())
      {
        ready = false;
        AString str("AObjectServer HTTPS on port ");
        str.append(AString::fromInt(https_port));
        str.append(" missing cert element, secure socket not listening.");
        m_Services.useConfiguration().emit(str);
        AOS_DEBUGTRACE(str.c_str(), NULL);
      }
      if (p->pkey.isEmpty())
      {
        ready = false;
        AString str("AObjectServer HTTPS on port ");
        str.append(AString::fromInt(https_port));
        str.append(" missing pkey element, secure socket not listening.");
        AOS_DEBUGTRACE(str.c_str(), NULL);
      }

      if (!ASocketLibrary::canBindToPort(https_port))
      {
        ready = false;
        AString str("Unable to bind to https port ");
        str.append(AString::fromInt(https_port));
        str.append(", already in use.");
        AOS_DEBUGTRACE(str.c_str(), NULL);
      }

      if (ready)
      {
        mthread_SecureListener.setParameter(p);
        mthread_SecureListener.start();
      }
    }
  }
  else
  {
    AOS_DEBUGTRACE("HTTPS disabled, not listening.", NULL);
  }
}

void AOSRequestListener::stopListening()
{
  mthread_Listener.setRun(false);
  mthread_SecureListener.setRun(false);
}

u4 AOSRequestListener::threadprocListener(AThread& thread)
{
  AAutoPtr<LISTEN_DATA> pData(static_cast<LISTEN_DATA *>(thread.getParameter()));
  AASSERT(NULL, pData.get());
  AOSRequestListener *pThis = pData->pListener;
  AASSERT(NULL, pThis);

  thread.setRunning(true);
  AOSContext *pContext = NULL;
  try
  {
    ASocketListener listener(pData->port, pData->host);
    listener.open();

    pThis->m_Services.useLog().add(ARope("AObjectServer started.  HTTP listening on ")+pData->host+ASW(":",1)+AString::fromInt(pData->port));
    {
      AString str("AObjectServer HTTP listening on ");
      str.append(pData->host);
      str.append(':');
      str.append(AString::fromInt(pData->port));
      AOS_DEBUGTRACE(str.c_str(), NULL);
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
    AString str("AObjectServer HTTP listener thread caught an exception: ");
    str.append(e);
    pThis->m_Services.useLog().add(e);
    AOS_DEBUGTRACE(str.c_str(), NULL);
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
  }
  catch(...)
  {
    AOS_DEBUGTRACE("AObjectServer HTTP listener thread caught an unknown exception.", NULL);
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
  }

  thread.setRunning(false);
  return 0;
}

u4 AOSRequestListener::threadprocSecureListener(AThread& thread)
{
  AAutoPtr<LISTEN_DATA> pData(static_cast<LISTEN_DATA *>(thread.getParameter()));
  AASSERT(NULL, pData.get());
  AOSRequestListener *pThis = pData->pListener;
  AASSERT(NULL, pThis);

  ASocketListener_SSL listener(
    pData->port,
    pData->cert,
    pData->pkey,
    pData->host
  );
  listener.open();

  pThis->m_Services.useLog().add(ARope("AObjectServer started.  HTTPS listening on ")+pData->host+ASW(":",1)+AString::fromInt(pData->port));
  {
    AString str("AObjectServer HTTPS listening on ");
    str.append(pData->host);
    str.append(':');
    str.append(AString::fromInt(pData->port));
    AOS_DEBUGTRACE(str.c_str(), NULL);
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
    AString str("AObjectServer HTTPS listener thread caught an exception: ");
    str.append(e);
    pThis->m_Services.useLog().add(e);
    AOS_DEBUGTRACE(str.c_str(), NULL);
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
  }
  catch(...)
  {
    AOS_DEBUGTRACE("AObjectServer HTTPS listener thread caught an unknown exception.", NULL);
    thread.setRunning(false);
    pThis->m_Services.useContextManager().deallocate(pContext);
  }

  thread.setRunning(false);
  return 0;
}

bool AOSRequestListener::isRunning() const
{
  return (mthread_Listener.isRunning() && mthread_SecureListener.isRunning());
}
