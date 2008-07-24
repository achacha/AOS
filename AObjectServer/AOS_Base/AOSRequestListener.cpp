/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSRequestListener.hpp"
#include "ASocketListener.hpp"
#include "ASocketListener_SSL.hpp"
#include "AFile_Socket_SSL.hpp"
#include "ASocketException.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"

#define WAIT_FOR_CONNECTION_DELAY 100

const AString& AOSRequestListener::getClass() const
{
  static const AString CLASS("AOSRequestListener");
  return CLASS;
}

void AOSRequestListener::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  adminAddProperty(eBase, ASW("first_queue",11), AString::fromInt(m_FirstQueue));
  adminAddProperty(eBase, ASW("listener_running",16), (mthread_Listener.isRunning() ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE));
  adminAddProperty(eBase, ASW("secure_listener_running",23), (mthread_SecureListener.isRunning() ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE));
  adminAddProperty(eBase, ASW("listener_run",12), (mthread_Listener.isRun() ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE));
  adminAddProperty(eBase, ASW("secure_listener_run",19), (mthread_Listener.isRun() ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE));
}

AOSRequestListener::AOSRequestListener(
  AOSServices& services,
  AOSContextManager::ContextQueueState firstQueue
) :
  mthread_Listener(AOSRequestListener::threadprocListener),
  mthread_SecureListener(AOSRequestListener::threadprocSecureListener),
  m_Services(services),
  m_FirstQueue(firstQueue)
{
  adminRegisterObject(m_Services.useAdminRegistry());
}

AOSRequestListener::~AOSRequestListener()
{
}

void AOSRequestListener::startListening()
{
  if (mthread_Listener.isThreadActive() || mthread_SecureListener.isThreadActive())
    ATHROW(this, AException::ProgrammingError);

  //a_HTTP setup
  AXmlElement *pNode = m_Services.useConfiguration().useConfigRoot().findElement(ASW("/config/server/listen/http",26));
  
  {
    int http_port = -1;
    if (pNode->findElement(ASW("port",4)))
    {
      http_port = pNode->getInt(ASW("port",4), -1);
    }
    if (http_port > 0)
    {
      if (!ASocketLibrary::canBindToPort(http_port))
      {
        AString str("Unable to bind to http port ");
        str.append(AString::fromInt(http_port));
        str.append(", already in use.");
        AOS_DEBUGTRACE(str.c_str(), NULL);
      }
      else
      {
        LISTEN_DATA *p = new LISTEN_DATA();
        p->port = http_port;
        pNode->emitString(ASW("host",4), p->host);

        mthread_Listener.setThis(this);
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
  pNode = m_Services.useConfiguration().useConfigRoot().findElement(ASW("/config/server/listen/https",27));
  
  {
    int https_port = -1;
    if (pNode->findElement(ASW("port",4)))
    {
      https_port = pNode->getInt(ASW("port",4), -1);
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
        pNode->emitString(ASW("host",4), p->host);
        pNode->emitString(ASW("cert",4), p->cert); 
        pNode->emitString(ASW("pkey",4), p->pkey);

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
          mthread_SecureListener.setThis(this);
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
  AOSRequestListener *pThis = dynamic_cast<AOSRequestListener *>(thread.getThis());
  AAutoPtr<LISTEN_DATA> pData(dynamic_cast<LISTEN_DATA *>(thread.getParameter()), true);

  AASSERT(NULL, pThis);
  AASSERT(NULL, pData.get());

  static bool httpBlockingMode = pThis->m_Services.useConfiguration().useConfigRoot().getBool(ASW("/config/server/listen/http/blocking",35), false);

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
        AAutoPtr<AFile_Socket> pSocket(NULL, false);
        try
        {
          pSocket.reset(new AFile_Socket(listener, httpBlockingMode));
          pSocket->open();
          pContext = pThis->m_Services.useContextManager().allocate(pSocket.use());
          pSocket.setOwnership(false);
          pThis->m_Services.useContextManager().changeQueueState(pThis->m_FirstQueue, &pContext);
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
          if (pContext)
            pThis->m_Services.useContextManager().deallocate(pContext);

          pContext = NULL;
          pSocket.reset();

          throw;
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
  AOSRequestListener *pThis = dynamic_cast<AOSRequestListener *>(thread.getThis());
  AAutoPtr<LISTEN_DATA> pData(dynamic_cast<LISTEN_DATA *>(thread.getParameter()), true);
  AASSERT(NULL, pThis);
  AASSERT(NULL, pData.get());

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
        AAutoPtr<AFile_Socket> pSocket(NULL, false);
        try
        {
          pSocket.reset(new AFile_Socket_SSL(listener));
          pSocket->open();
          pContext = pThis->m_Services.useContextManager().allocate(pSocket.use());
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

        pThis->m_Services.useContextManager().changeQueueState(pThis->m_FirstQueue, &pContext);
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
