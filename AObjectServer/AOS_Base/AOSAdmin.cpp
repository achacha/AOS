/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSAdmin.hpp"
#include "AString.hpp"
#include "AFile_Socket.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ALock.hpp"
#include "ASocketListener.hpp"
#include "AXmlDocument.hpp"
#include "AXmlInstruction.hpp"
#include "AOSServices.hpp"
#include "AOSRequestListener.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"

#include "AOSAdminCommand_version.hpp"
#include "AOSAdminCommand_list.hpp"
#include "AOSAdminCommand_display.hpp"
#include "AOSAdminCommand_modify.hpp"

AOSAdmin::AOSAdmin(
  AOSServices& services
) :
  m_ClientSynch("AdminClient"),
  m_Services(services),
  m_AdminCommandRegistry(services),
  m_IsShutdownRequested(false),
  mthread_AdminListener(AOSAdmin::threadprocAdminListener, false)
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
  //a_Initialize the commands
  m_AdminCommandRegistry.insert(new AOSAdminCommand_version(m_Services));
  m_AdminCommandRegistry.insert(new AOSAdminCommand_list(m_Services));
  m_AdminCommandRegistry.insert(new AOSAdminCommand_display(m_Services));
  m_AdminCommandRegistry.insert(new AOSAdminCommand_modify(m_Services));
  
  //a_Get port and start listener thread
  if (m_Services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/listen/admin/port"), -1) > 0)
  {
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
        size_t ret = client.readLine(str);
        while (AConstant::npos == ret || AConstant::unavail == ret)
        {
          AThread::sleep(50);
          if (!client.isOpen() || !client.isNotEof())
            continue;

          ret = client.readLine(str);
        }
        if (!request.parseLineZero(str))
        {
          pThis->m_Services.useLog().add(ASWNL("AOSAdmin::threadprocAdminListener: Unable to parse line 0"), str, ALog::FAILURE);
          continue;
        }

        str.clear();
        while (client.isOpen() && client.isNotEof())
        {
          size_t readRet = client.readLine(str);

          if (AConstant::unavail == readRet)
          {
            std::cerr << "." << std::flush;
            continue;
          }
          
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

        //a_Parse the post
        if (request.isFormPost())
        {
          str.clear();
          size_t length = request.getContentLength();
          size_t readData = client.read(str, length);
          request.useUrl().useParameterPairs().parse(str);
          AASSERT(&str, length == readData);
        }

        pThis->_processRequest(client, request);
      }
    }
    catch(AException& ex)
    {
      pThis->m_Services.useLog().addException(ex);
    }
    catch(std::exception& ex)
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
      response.set(AHTTPResponseHeader::HT_ENT_Content_Type, str);
    else
      response.set(AHTTPResponseHeader::HT_ENT_Content_Type, ASW("text/plain",10));

    response.set(AHTTPHeader::HT_RES_Server, AOS_ADMIN_SERVER_NAME);
    response.set(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(outputBuffer.getSize()));

    response.emit(client);
    outputBuffer.emit(client);
  }
}

void AOSAdmin::_processAdmin(AFile_Socket& client, AHTTPRequestHeader& request)
{
  ARope outputBuffer;
  AString command;
  AHTTPResponseHeader response;
  response.set(AHTTPHeader::HT_RES_Server, AOS_ADMIN_SERVER_NAME);

  if (!request.useUrl().useParameterPairs().get(ASW("command",7), command))
  {
    //a_No command
    response.set(AHTTPHeader::HT_RES_Location, ASW("/",1));
    response.setStatusCode(AHTTPResponseHeader::SC_302_Moved_Temporarily);
  }
  else
  {
    if (command.equals("shutdown"))
      _shutdown(request, response, outputBuffer);
    else
    {
      //a_Command processor specified
      AOSAdminCommandInterface *pCommand = m_AdminCommandRegistry.get(command);
      if (!pCommand)
      {
        m_Services.useLog().add(ASWNL("AOSAdmin::_processAdminCommand"), ASWNL("Unable to find a registered command"), command);
        
        AXmlDocument xmlDoc(ASW("admin",5));
        xmlDoc.useRoot().addElement(ASW("error",5)).addData(ARope("Command not found: ")+command, AXmlElement::ENC_CDATADIRECT);
        xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
          .addAttribute(ASW("type",4), ASW("text/xsl",8))
          .addAttribute(ASW("href",4), ASW("/xsl/_command/error.xsl",23));

        response.setStatusCode(AHTTPResponseHeader::SC_400_Bad_Request);
        xmlDoc.emit(outputBuffer);
      }
      else
      {
        AOSAdminCommandContext context(request, response, outputBuffer);
        pCommand->process(context);
      }
    }
  }

  //a_Generate response header and emit
  response.set(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(outputBuffer.getSize()));
  response.set(AHTTPHeader::HT_GEN_Cache_Control, ASW("no-cache",8));
  response.set(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml; charset=utf-8",23));
  response.emit(client);
  
  //a_Emit response
  outputBuffer.emit(client);
}

void AOSAdmin::_shutdown(
  AHTTPRequestHeader& request,
  AHTTPResponseHeader& response,
  AOutputBuffer& outputBuffer
)
{
  AString str(2048, 1024);
  AXmlDocument xmlDoc(ASW("admin",5));
  response.set(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml; charset=utf-8",23));

  AXmlElement& elem = xmlDoc.useRoot().addElement(ASW("shutdown",8));

  //a_Shutdown
  //a_Flag admin listener to stop
  mthread_AdminListener.setRun(false);
  m_IsShutdownRequested = true;

  m_Services.useLog().add(ASW("ADMIN: Shutdown",15), ALog::INFO);
  AOS_DEBUGTRACE("Shutdown request received... initiating shutdown sequence.", NULL);

  AOS_DEBUGTRACE("Trying to stop AOSRequestListener...", NULL);
  AOSRequestListener *pListener = dynamic_cast<AOSRequestListener *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSRequestListener")));
  AASSERT(NULL, pListener);
  pListener->stopListening();
  AOS_DEBUGTRACE("AOSRequestListener stopped.", NULL);
  elem.addElement(ASWNL("AOSRequestListener"));

  AOS_DEBUGTRACE("Trying to stop AOSRequestQueue_IsAvailable...", NULL);
  AOSContextQueue_IsAvailable *pcqIsAvailable = dynamic_cast<AOSContextQueue_IsAvailable *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_IsAvailable")));;
  AASSERT(NULL, pcqIsAvailable);
  pcqIsAvailable->stop();
  AOS_DEBUGTRACE("AOSRequestQueue_IsAvailable stopped.", NULL);
  elem.addElement(ASWNL("AOSContextQueue_IsAvailable"));

  AOS_DEBUGTRACE("Trying to stop AOSContextQueue_PreExecutor...", NULL);
  AOSContextQueue_PreExecutor *pcqPreExecutor = dynamic_cast<AOSContextQueue_PreExecutor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_PreExecutor")));;
  AASSERT(NULL, pcqPreExecutor);
  pcqPreExecutor->stop();
  AOS_DEBUGTRACE("AOSContextQueue_PreExecutor stopped.", NULL);
  elem.addElement(ASWNL("AOSContextQueue_PreExecutor"));

  AOS_DEBUGTRACE("Trying to stop AOSContextQueue_Executor...", NULL);
  AOSContextQueue_Executor *pcqExecutor = dynamic_cast<AOSContextQueue_Executor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_Executor")));;
  AASSERT(NULL, pcqExecutor);
  pcqExecutor->stop();
  AOS_DEBUGTRACE("AOSContextQueue_Executor stopped.", NULL);
  elem.addElement(ASWNL("AOSContextQueue_Executor"));

  AOS_DEBUGTRACE("Trying to stop AOSContextQueue_ErrorExecutor...", NULL);
  AOSContextQueue_ErrorExecutor *pcqErrorExecutor = dynamic_cast<AOSContextQueue_ErrorExecutor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_ErrorExecutor")));;
  AASSERT(NULL, pcqErrorExecutor);
  pcqErrorExecutor->stop();
  AOS_DEBUGTRACE("AOSContextQueue_ErrorExecutor stopped.", NULL);
  elem.addElement(ASWNL("AOSContextQueue_ErrorExecutor"));

  //a_Emit result
  xmlDoc.emit(outputBuffer);
}

bool AOSAdmin::isShutdownRequested() const
{
  return m_IsShutdownRequested;
}
