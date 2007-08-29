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

  //a_HTTP setup
  AXmlNode *pNode = m_Services.useConfiguration().useConfigRoot().findNode(ASW("/config/server/listen/http",26));
  
  {
    int http_port = -1;
    if (pNode->findNode(ASW("port",4)))
    {
      AString str;
      pNode->emitFromPath(ASW("port",4), str);
      http_port = str.toInt();
    }
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
        pNode->emitFromPath(ASW("host",4), p->host);

        mthread_Listener.setParameter(p);
        mthread_Listener.start();
      }
    }
    else
    {
      AString str("HTTP invalid port specified:");
      str.append(AString::fromInt(http_port));
      AOS_DEBUGTRACE(str.c_str(), NULL);
    }
  }
  
  //a_HTTPS setup
  pNode = m_Services.useConfiguration().useConfigRoot().findNode(ASW("/config/server/listen/https",27));
  
  {
    int https_port = -1;
    if (pNode->findNode(ASW("port",4)))
    {
      AString str;
      pNode->emitFromPath(ASW("port",4), str);
      https_port = str.toInt();
    }
    if (https_port > 0)
    {
      bool ready = true;
      if (!pNode)
      {
        AOS_DEBUGTRACE("AObjectServer HTTPS element not found, secure socket not listening.", NULL);
      }
      else
      {
        LISTEN_DATA *p = new LISTEN_DATA();
        p->port = https_port;
        p->pListener = this;
        pNode->emitFromPath(ASW("host",4), p->host);
        pNode->emitFromPath(ASW("cert",4), p->cert); 
        pNode->emitFromPath(ASW("pkey",4), p->pkey);

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
      AString str("HTTPS invalid port specified:");
      str.append(AString::fromInt(https_port));
      AOS_DEBUGTRACE(str.c_str(), NULL);
    }
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
      str.append(listener.getSocketInfo().m_address);
      str.append(':');
      str.append(AString::fromInt(listener.getSocketInfo().m_port));
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
    str.append(listener.getSocketInfo().m_address);
    str.append(':');
    str.append(AString::fromInt(listener.getSocketInfo().m_port));
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
