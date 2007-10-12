#include "pchAOS_Base.hpp"
#include "AOSAdmin.hpp"
#include "AString.hpp"
#include "AFile_Socket.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ALock.hpp"
#include "ASocketListener.hpp"
#include "AXmlDocument.hpp"
#include "AOSAdminInterface.hpp"
#include "AXmlInstruction.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"
#include "AOSRequestListener.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"

const char _AOS_ADMIN_BUILD_INFO_[] = "AOS Admin \tBUILD(" __TIME__ " " __DATE__ ")";

AOSAdmin::AOSAdmin(
  AOSServices& services
) :
  m_ClientSynch("AdminClient"),
  m_Services(services),
  m_IsShutdownRequested(false)
{
}

bool AOSAdmin::isRun()
{
  return mthread_AdminListener.isRun();
}

bool AOSAdmin::isRunning()
{
  return mthread_AdminListener.isRunning();
}

void AOSAdmin::startAdminListener()
{
  if (m_Services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/listen/admin/port"), -1) > 0)
  {
    mthread_AdminListener.setProc(AOSAdmin::threadprocAdminListener);
    mthread_AdminListener.setThis(this);
    mthread_AdminListener.start();
  }
  else
  {
    AOS_DEBUGTRACE("Admin server disabled, not listening.", NULL);
  }

  return;
}

void AOSAdmin::stopAdminListener()
{
  mthread_AdminListener.setRun(false);
}

u4 AOSAdmin::threadprocAdminListener(AThread& thread)
{
  AOSAdmin *pThis = dynamic_cast<AOSAdmin *>(thread.getThis());
  AASSERT(NULL, pThis);
  if (!pThis)
    return -1;

  int admin_port = pThis->m_Services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/listen/admin/port"), 12345);
  if (-1 != admin_port && !ASocketLibrary::canBindToPort(admin_port))
  {
    AString str("main: Unable to bind to admin port ");
    str.append(AString::fromInt(admin_port));
    str.append(" already in use.");
    AOS_DEBUGTRACE(str.c_str(), NULL);
    return -1;
  }

  AString hostname;
  pThis->m_Services.useConfiguration().useConfigRoot().emitString(ASWNL("/config/server/listen/admin/host"), hostname);
  ASocketListener listener(admin_port, ASocketLibrary::getIPFromAddress(hostname));
  try
  {
    listener.open();
  }
  catch(AException& ex)
  {
    AOS_DEBUGTRACE(ex.what().toAString().c_str(), NULL);
    return -1;
  }

  
  AString str("AObjectServer HTTP admin listening on ");
  str.append(listener.getSocketInfo().m_address);
  str.append(':');
  str.append(AString::fromInt(listener.getSocketInfo().m_port));
  pThis->m_Services.useLog().add(str, ALog::MESSAGE);
  AOS_DEBUGTRACE(str.c_str(), NULL);

  str.clear();
  thread.setRunning(true);
  while(thread.isRun())
  {
    try
    {
      AFile_Socket client(listener, true);
      client.open();
      {
        ALock lock(pThis->m_ClientSynch);

        str.clear();
        AHTTPRequestHeader request;
        while (AConstant::npos == client.readLine(str))
        {
          AThread::sleep(50);
        }
        if (!request.parseLineZero(str))
        {
          pThis->m_Services.useLog().add(ASWNL("AOSAdmin::threadprocAdminListener: Unable to parse line 0"), str, ALog::FAILURE);
          break;
        }

        str.clear();
        while (client.isOpen() && client.isNotEof())
        {
          size_t readRet = client.readLine(str);
          
          //a_End of HTTP header (blank line)
          if (!readRet)
            break;

          if (readRet != AConstant::npos)
          {
            request.parseTokenLine(str);
            str.clear();
          }
          else
          {
            AThread::sleep(50);
          }
        }

        pThis->_processRequest(client, request);
      }
    }
    catch(AException& ex)
    {
      pThis->m_Services.useLog().addException(ex);
    }
    catch(...)
    {
      pThis->m_Services.useLog().add(ASWNL("AOSAdmin::threadprocAdminListener: Unknown exception caught."), ALog::EXCEPTION);
    }
  }

  thread.setRunning(false);

  AOS_DEBUGTRACE("AObjectServerAdmin thread exited.", NULL);
  return 0;
}

void AOSAdmin::_processRequest(AFile_Socket& client, AHTTPRequestHeader& request)
{
  if (request.useUrl().getPathAndFilename().equals("/admin"))
  {
    _processAdmin(client, request);
  }
  else
  {
    //a_Serve page
    AString strExt;
    AFilename httpFileServe(m_Services.useConfiguration().getAdminBaseHttpDir(), request.useUrl().getPath(), true);
    if (request.useUrl().getFilename().isEmpty())
    {
      httpFileServe.useFilename().assign("index.html", 10);
      strExt.assign("html",4);
    }
    else
    {
      httpFileServe.useFilename().assign(request.useUrl().getFilename());
      strExt.assign(request.useUrl().getExtension());
    }
    
    AHTTPResponseHeader response;
    ARope outputBuffer;
    if (AFileSystem::exists(httpFileServe))
    {
      //a_Send the file
      AFile_Physical localFile(httpFileServe.toAString(), ASW("rb",2));
      localFile.open();
      m_Services.useLog().add(ASW("ADMIN: Static file",18), httpFileServe, ALog::INFO);
      localFile.readUntilEOF(outputBuffer);
      localFile.close();
    }
    else
    {
      //a_File not found
      m_Services.useLog().add(ASW("ADMIN: Static file NOT FOUND",28), httpFileServe, ALog::INFO);
      response.setStatusCode(AHTTPResponseHeader::SC_404_Not_Found);
      outputBuffer.append("File not found: ",16);
      outputBuffer.append(httpFileServe);
    }

    //a_Set MIME type from extension
    AString str(64,16);
    if (m_Services.useConfiguration().getMimeTypeFromExt(strExt, str))
      response.setPair(AHTTPResponseHeader::HT_ENT_Content_Type, str);
    else
      response.setPair(AHTTPResponseHeader::HT_ENT_Content_Type, ASW("text/plain",10));

    response.setPair(AHTTPHeader::HT_RES_Server, AOS_ADMIN_SERVER_NAME);
    response.setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(outputBuffer.getSize()));
    response.setPair(AHTTPHeader::HT_GEN_Cache_Control, ASW("no-cache",8));

    response.emit(client);
    outputBuffer.emit(client);
  }
}

void AOSAdmin::_processAdmin(AFile_Socket& client, AHTTPRequestHeader& request)
{
  ARope outputBuffer;
  AString command;
  AHTTPResponseHeader response;
  response.setPair(AHTTPHeader::HT_RES_Server, AOS_ADMIN_SERVER_NAME);

  if (!request.useUrl().useParameterPairs().get(ASW("command",7), command))
  {
    //a_No command
    response.setPair(AHTTPHeader::HT_RES_Location, ASW("/",1));
    response.setStatusCode(AHTTPResponseHeader::SC_302_Moved_Temporarily);
  }
  else
  {
    _processAdminCommand(command, request, response, outputBuffer);
  }

  //a_Generate response header and emit
  response.setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(outputBuffer.getSize()));
  response.emit(client);
  
  //a_Emit response
  outputBuffer.emit(client);
}

void AOSAdmin::_processAdminCommand(
  const AString& command,
  AHTTPRequestHeader& request,
  AHTTPResponseHeader& response,
  AOutputBuffer& outputBuffer
)
{
  AString str(2048, 1024);
  AXmlDocument xmlDoc(ASW("admin",5));
  _prepareXmlDocument(command, request, xmlDoc);
  response.setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml",8));

  try
  {
    if (command.equalsNoCase(ASW("version",7)))
    {
      xmlDoc.useRoot().addElement(ASW("version",7), _AOS_ADMIN_BUILD_INFO_);
    }
    else if (command.equalsNoCase(ASW("list",4)))
    {
      //a_Display list of objects
      LIST_AString objectNames;
      m_Services.useAdminRegistry().listAdminObjects(objectNames);

      LIST_AString::iterator it = objectNames.begin();
      while (it != objectNames.end())
      {
        AXmlElement& e = xmlDoc.useRoot().addElement(ASW("object",6));
        e.addAttribute(ASW("name",4), *it);

        ++it;
      }
    }
    else if (command.equalsNoCase(ASW("display",7)))
    {
      AString objectName;
      if (!request.useUrl().useParameterPairs().get(ASW("object",6), objectName))
      {
        xmlDoc.useRoot().addElement(ASW("error",5), ASW("Please specify which 'object=' to display",41));
      }
      else
      {
        m_Services.useLog().add(ASW("ADMIN: Display object",21), objectName, ALog::INFO);

        //a_Display object specific list
        AOSAdminInterface *pAdminObject = m_Services.useAdminRegistry().getAdminObject(objectName);
        
        //a_Add methods if any
        if (pAdminObject)
        {
          AXmlElement& eObject = xmlDoc.useRoot().addElement(ASW("object",6)).addAttribute(ASW("name",4), objectName);
          pAdminObject->addAdminXml(eObject, request);
        }
        else
        {
          xmlDoc.useRoot().addElement(ASW("error",5), ASW("Unknown object",14));
        }
      }
    }
    else if (command.equalsNoCase(ASW("modify",6)))
    {
      AString objectName;
      if (!request.useUrl().useParameterPairs().get(ASW("object",6), objectName))
      {
        xmlDoc.useRoot().addElement(ASW("error",5), ASW("Please specify which 'object=' to display",41));
      }
      else
      {
        m_Services.useLog().add(ASW("ADMIN: Modify object",20), objectName, ALog::INFO);

        //a_Display object specific list
        AOSAdminInterface *pAdminObject = m_Services.useAdminRegistry().getAdminObject(objectName);
        
        //a_Add methods if any
        if (pAdminObject)
        {
          AXmlElement& eObject = xmlDoc.useRoot().addElement(ASW("object",6)).addAttribute(ASW("name",4), objectName);
          pAdminObject->processAdminAction(eObject, request);
          pAdminObject->addAdminXml(eObject, request);
        }
        else
        {
          xmlDoc.useRoot().addElement(ASW("error",5), ASW("Unknown object",14));
        }
      }
    }
    else if (command.equalsNoCase(ASW("shutdown",8)))
    {
      //a_Shutdown
      m_Services.useLog().add(ASW("ADMIN: Shutdown",15), ALog::INFO);
      AOS_DEBUGTRACE("Shutdown request received... initiating shutdown sequence.", NULL);

      AOS_DEBUGTRACE("Trying to stop AOSRequestListener...", NULL);
      AOSRequestListener *pListener = dynamic_cast<AOSRequestListener *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSRequestListener")));
      AASSERT(NULL, pListener);
      pListener->stopListening();
      AOS_DEBUGTRACE("AOSRequestListener stopped.", NULL);

      AOS_DEBUGTRACE("Trying to stop AOSRequestQueue_IsAvailable...", NULL);
      AOSContextQueue_IsAvailable *pcqIsAvailable = dynamic_cast<AOSContextQueue_IsAvailable *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_IsAvailable")));;
      AASSERT(NULL, pcqIsAvailable);
      pcqIsAvailable->useThreadPool().stop();
      AOS_DEBUGTRACE("AOSRequestQueue_IsAvailable stopped.", NULL);

      AOS_DEBUGTRACE("Trying to stop AOSContextQueue_PreExecutor...", NULL);
      AOSContextQueue_PreExecutor *pcqPreExecutor = dynamic_cast<AOSContextQueue_PreExecutor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_PreExecutor")));;
      AASSERT(NULL, pcqPreExecutor);
      pcqPreExecutor->useThreadPool().stop();
      AOS_DEBUGTRACE("AOSContextQueue_PreExecutor stopped.", NULL);

      AOS_DEBUGTRACE("Trying to stop AOSContextQueue_Executor...", NULL);
      AOSContextQueue_Executor *pcqExecutor = dynamic_cast<AOSContextQueue_Executor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_Executor")));;
      AASSERT(NULL, pcqExecutor);
      pcqExecutor->useThreadPool().stop();
      AOS_DEBUGTRACE("AOSContextQueue_Executor stopped.", NULL);

      AOS_DEBUGTRACE("Trying to stop AOSContextQueue_ErrorExecutor...", NULL);
      AOSContextQueue_ErrorExecutor *pcqErrorExecutor = dynamic_cast<AOSContextQueue_ErrorExecutor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_ErrorExecutor")));;
      AASSERT(NULL, pcqErrorExecutor);
      pcqErrorExecutor->useThreadPool().stop();
      AOS_DEBUGTRACE("AOSContextQueue_ErrorExecutor stopped.", NULL);

      //a_Emit result
      xmlDoc.useRoot().addElement(ASW("shutdown",8));

      //a_Flag admin listener to stop
      mthread_AdminListener.setRun(false);
      m_IsShutdownRequested = true;
    }
    else
    {
      xmlDoc.useRoot().addElement(ASW("error",5), ARope("Unknown command: '",18)+command+"'");
    }
  }
  catch(AException& ex)
  {
    m_Services.useLog().add(ASW("ADMIN: Exception",16), ex, ALog::WARNING);
    xmlDoc.useRoot().addElement("exception", ex.what());
  }
  catch(...)
  {
    m_Services.useLog().add(ASW("ADMIN: Exception unknown",24), ALog::WARNING);
    xmlDoc.useRoot().addElement("exception", "Unknown");
  }

  xmlDoc.emit(outputBuffer);
}

void AOSAdmin::_prepareXmlDocument(
  const AString& command,
  AHTTPRequestHeader& request,
  AXmlDocument& xmlDoc
)
{
  //a_Determine which xsl to use, omit XSL directive if XML only
  if (!request.useUrl().useParameterPairs().exists(ASW("xml",3)))
  {
    if (command.equalsNoCase(ASW("list",4)))
    {
      xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
        .addAttribute(ASW("type",4), ASW("text/xsl",8))
        .addAttribute(ASW("href",4), ASWNL("/xsl/_command/list.xsl"));
    }
    else if (command.equalsNoCase("display"))
    {
      AString objectName;
      if (request.useUrl().useParameterPairs().get("object", objectName))
      {
        if (objectName.equals("AOSContextManager"))
        {
          xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
            .addAttribute(ASW("type",4), ASW("text/xsl",8))
            .addAttribute(ASW("href",4), ASW("/xsl/_command/display_AOSContextManager.xsl",43));
        }
        else
        {
          xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
            .addAttribute(ASW("type",4), ASW("text/xsl",8))
            .addAttribute(ASW("href",4), ASW("/xsl/_command/display.xsl",25));
        }
      }
      else
      {
        //a_This operation requires an object
        xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
          .addAttribute(ASW("type",4), ASW("text/xsl",8))
          .addAttribute(ASW("href",4), ASW("/xsl/_command/display.xsl",25));
      }
    }
    else if (command.equalsNoCase(ASW("modify",6)))
    {
      AString objectName;
      if (request.useUrl().useParameterPairs().get(ASW("object",6), objectName))
      {
        xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
          .addAttribute(ASW("type",4), ASW("text/xsl",8))
          .addAttribute(ASW("href",4), ASW("/xsl/_command/display.xsl",25));
      }
      else
      {
        //a_This operation requires an object
        xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
          .addAttribute(ASW("type",4), ASW("text/xsl",8))
          .addAttribute(ASW("href",4), ASW("/xsl/_command/display.xsl",25));
      }
    }
  }
}

bool AOSAdmin::isShutdownRequested() const
{
  return m_IsShutdownRequested;
}
