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
  m_Services(services)
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
  if (m_Services.useConfiguration().getIntWithDefault(AOSConfiguration::LISTEN_ADMIN_PORT, -1) > 0)
  {
    mthread_AdminListener.setProc(AOSAdmin::threadprocAdminListener);
    mthread_AdminListener.setParameter(this);
    mthread_AdminListener.start();
  }
  else
  {
    ALock lock(m_Services.useScreenSynch());
    std::cout << "Admin server disabled, not listening." << std::endl;
  }

  return;
}

void AOSAdmin::stopAdminListener()
{
  mthread_AdminListener.setRun(false);
}

u4 AOSAdmin::threadprocAdminListener(AThread& thread)
{
  thread.setRunning();

  AOSAdmin *pThis = (AOSAdmin *)thread.getParameter();
  AASSERT(NULL, pThis);

  AString str;
  int admin_port = pThis->m_Services.useConfiguration().getInt(AOSConfiguration::LISTEN_ADMIN_PORT);
  ASocketListener listener(admin_port);
  try
  {
    listener.open();
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
    thread.setRunning(false);
    return -1;
  }

  pThis->m_Services.useLog().add(ARope("HTTP admin listening on port ")+AString::fromInt(admin_port), ALog::MESSAGE);
  
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer HTTP admin listening on port " << admin_port << std::endl;
  }
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
          AThread::sleep(5);
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
            AThread::sleep(20);
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

  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServerAdmin thread exiting." << std::endl;
  }
  thread.setRunning(false);
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
    AFilename httpFileServe(m_Services.useConfiguration().getAdminBaseHttpDirectory(), request.useUrl().getPath());
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
      ALock lock(m_Services.useScreenSynch());
      m_Services.useLog().add(ASW("ADMIN: Shutdown",15), ALog::INFO);
      std::cout << "Shutdown request received... initiating shutdown sequence." << std::endl;

      std::cout << "Trying to stop AOSRequestListener..." << std::flush;
      AOSRequestListener *pListener = dynamic_cast<AOSRequestListener *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSRequestListener")));
      AASSERT(NULL, pListener);
      pListener->stopListening();
      std::cout << "STOPPED." << std::endl;

      std::cout << "Trying to stop AOSRequestQueue_IsAvailable..." << std::flush;
      AOSContextQueue_IsAvailable *pcqIsAvailable = dynamic_cast<AOSContextQueue_IsAvailable *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_IsAvailable")));;
      AASSERT(NULL, pcqIsAvailable);
      pcqIsAvailable->useThreadPool().stop();
      std::cout << "STOPPED." << std::endl;

      std::cout << "Trying to stop AOSContextQueue_PreExecutor..." << std::flush;
      AOSContextQueue_PreExecutor *pcqPreExecutor = dynamic_cast<AOSContextQueue_PreExecutor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_PreExecutor")));;
      AASSERT(NULL, pcqPreExecutor);
      pcqPreExecutor->useThreadPool().stop();
      std::cout << "STOPPED." << std::endl;

      std::cout << "Trying to stop AOSContextQueue_Executor..." << std::flush;
      AOSContextQueue_Executor *pcqExecutor = dynamic_cast<AOSContextQueue_Executor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_Executor")));;
      AASSERT(NULL, pcqExecutor);
      pcqExecutor->useThreadPool().stop();
      std::cout << "STOPPED." << std::endl;

      std::cout << "Trying to stop AOSContextQueue_ErrorExecutor..." << std::flush;
      AOSContextQueue_ErrorExecutor *pcqErrorExecutor = dynamic_cast<AOSContextQueue_ErrorExecutor *>(m_Services.useAdminRegistry().getAdminObject(ASWNL("AOSContextQueue_ErrorExecutor")));;
      AASSERT(NULL, pcqErrorExecutor);
      pcqErrorExecutor->useThreadPool().stop();
      std::cout << "STOPPED." << std::endl;

      //a_Flag admin listener to stop
      mthread_AdminListener.setRun(false);

      //a_Emit result
      xmlDoc.useRoot().addElement(ASW("shutdown",8));
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
