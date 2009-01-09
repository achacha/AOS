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

const AString AOSRequestListener::CLASS("AOSRequestListener");

const AString& AOSRequestListener::getClass() const
{
  return CLASS;
}

AOSRequestListener::LISTEN_DATA::LISTEN_DATA(AOSServices& services, const AString& configpath) : 
  m_port(-1)
{
  AXmlElement *pBase = services.useConfiguration().useConfigRoot().findElement(configpath);
  if (!pBase)
    ATHROW_EX(NULL, AException::InvalidConfiguration, configpath);

  pBase->emitContentFromPath(ASW("host",4), m_host);
  m_port = pBase->getInt(ASW("port",4), -1);

  AString str;
  if (pBase->emitContentFromPath(ASW("cert",4), str))
  {
    m_cert.set(services.useConfiguration().getBaseDir());
    m_cert.join(str, false);
  }

  str.clear();
  if (pBase->emitContentFromPath(ASW("pkey",4), str))
  {
    m_pkey.set(services.useConfiguration().getBaseDir());
    m_pkey.join(str, false);
  }
}

int AOSRequestListener::LISTEN_DATA::getPort() const
{
  return m_port;
}

const AString& AOSRequestListener::LISTEN_DATA::getHost() const
{
  return m_host;
}

const AFilename& AOSRequestListener::LISTEN_DATA::getCertificateFilename() const
{
  return m_cert;
}

const AFilename& AOSRequestListener::LISTEN_DATA::getPrivateKeyFilename() const
{
  return m_pkey;
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
  {
    if (m_Services.useConfiguration().useConfigRoot().exists(ASW("/config/server/listen/http",26)))
    {
      LISTEN_DATA *p = new LISTEN_DATA(m_Services, ASW("/config/server/listen/http",26));
      if (p->getPort() > 0)
      {
        if (!ASocketLibrary::canBindToPort(p->getPort()))
        {
          AString str("Unable to bind to HTTP port ");
          str.append(AString::fromInt(p->getPort()));
          str.append(", already in use.");
          AOS_DEBUGTRACE(str.c_str(), NULL);
          ATHROW_EX(this, ASocketException::UnableToOpen, str);
        }
        else
        {
          mthread_Listener.setThis(this);
          mthread_Listener.setParameter(p);
          mthread_Listener.start();
        }
      }
      else
      {
        AString str("HTTP invalid port specified:");
        str.append(AString::fromInt(p->getPort()));
        AOS_DEBUGTRACE(str.c_str(), NULL);
        ATHROW_EX(this, ASocketException::UnableToOpen, str);
      }
    }
  }
  
  //a_HTTPS setup
  {
    if (m_Services.useConfiguration().useConfigRoot().exists(ASW("/config/server/listen/https",27)))
    {
      LISTEN_DATA *p = new LISTEN_DATA(m_Services, ASW("/config/server/listen/https",27));
      if (p->getPort() > 0)
      {
        bool ready = true;
        if (p->getCertificateFilename().getFilename().isEmpty())
        {
          ready = false;
          AString str("AObjectServer HTTPS on port ");
          str.append(AString::fromInt(p->getPort()));
          str.append(" missing cert element, secure socket not listening.");
          AOS_DEBUGTRACE(str.c_str(), NULL);
          ATHROW_EX(this, ASocketException::UnableToOpen, str);
        }
        
        if (p->getPrivateKeyFilename().getFilename().isEmpty())
        {
          ready = false;
          AString str("AObjectServer HTTPS on port ");
          str.append(AString::fromInt(p->getPort()));
          str.append(" missing pkey element, secure socket not listening.");
          AOS_DEBUGTRACE(str.c_str(), NULL);
          ATHROW_EX(this, ASocketException::UnableToOpen, str);
        }

        if (!ASocketLibrary::canBindToPort(p->getPort()))
        {
          ready = false;
          AString str("Unable to bind to https port ");
          str.append(AString::fromInt(p->getPort()));
          str.append(", already in use.");
          AOS_DEBUGTRACE(str.c_str(), NULL);
          ATHROW_EX(this, ASocketException::UnableToOpen, str);
        }

        if (ready)
        {
          mthread_SecureListener.setThis(this);
          mthread_SecureListener.setParameter(p);
          mthread_SecureListener.start();
        }
      }
      else
      {
        AString str("HTTPS invalid port specified:");
        str.append(AString::fromInt(p->getPort()));
        AOS_DEBUGTRACE(str.c_str(), NULL);
        ATHROW_EX(this, ASocketException::UnableToOpen, str);
      }
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

  bool httpBlockingMode = pThis->m_Services.useConfiguration().useConfigRoot().getBool(ASW("/config/server/listen/http/blocking",35), false);
  int WAIT_FOR_CONNECTION_DELAY = pThis->m_Services.useConfiguration().useConfigRoot().getInt(ASW("/config/server/listen/http/listener-sleep",41), 50);
  AASSERT(pThis, WAIT_FOR_CONNECTION_DELAY > 0);

  thread.setRunning(true);
  AOSContext *pContext = NULL;
  try
  {
    ASocketListener listener(pData->getPort(), pData->getHost());
    listener.open();

    pThis->m_Services.useLog().add(ARope("AObjectServer started.  HTTP listening on ")+pData->getHost()+ASW(":",1)+AString::fromInt(pData->getPort()));
    {
      AString str("AObjectServer HTTP listening on ");
      str.append(listener.getSocketInfo().m_address);
      str.append(':');
      str.append(AString::fromInt(listener.getSocketInfo().m_port));
      str.append(" aos_root: ");
      str.append(pThis->m_Services.useConfiguration().getBaseDir());
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

  int WAIT_FOR_CONNECTION_DELAY = pThis->m_Services.useConfiguration().useConfigRoot().getInt(ASW("/config/server/listen/https/listener-sleep",42), 50);
  AASSERT(pThis, WAIT_FOR_CONNECTION_DELAY > 0);

  AString cert, pkey;
  pData->getCertificateFilename().emit(cert);
  pData->getPrivateKeyFilename().emit(pkey);
  ASocketListener_SSL listener(
    pData->getPort(),
    cert,
    pkey,
    pData->getHost()
    );
  listener.open();

  pThis->m_Services.useLog().add(ARope("AObjectServer started.  HTTPS listening on ")+pData->getHost()+ASW(":",1)+AString::fromInt(pData->getPort()));
  {
    AString str("AObjectServer HTTPS listening on ");
    str.append(listener.getSocketInfo().m_address);
    str.append(':');
    str.append(AString::fromInt(listener.getSocketInfo().m_port));
    str.append(" aos_root: ");
    str.append(pThis->m_Services.useConfiguration().getBaseDir());
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
