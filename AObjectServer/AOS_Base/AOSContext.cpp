#include "pchAOS_Base.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AOSCommand.hpp"
#include "ALock.hpp"
#include "AThread.hpp"
#include "AOSConfiguration.hpp"

const AString AOSContext::CONTEXT("context");
const AString AOSContext::XML_ROOT("root");
const AString AOSContext::OBJECTNAME("__AOSContext__");

void AOSContext::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSContext @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_EventVisitor=" << std::endl;
  m_EventVisitor.debugDump(os, indent+2);
  
  //a_Input file
  ADebugDumpable::indent(os, indent+1) << "*mp_File=" << std::endl;
  if (mp_RequestFile)
    mp_RequestFile->debugDump(os, indent+2);

  //a_Associated directory config
  if (mp_DirConfig)
  {
    ADebugDumpable::indent(os, indent+1) << "m_DirConfig=" << std::endl;
    mp_DirConfig->debugDump(os, indent+2);
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_DirConfig=NULL" << std::endl;

  //a_Associated command
  if (mp_Command)
  {
    ADebugDumpable::indent(os, indent+1) << "m_Command=" << std::endl;
    mp_Command->debugDump(os, indent+2);
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_Command=NULL" << std::endl;

  //a_HTTP data
  ADebugDumpable::indent(os, indent+1) << "m_RequestHeader=" << std::endl;
  m_RequestHeader.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_ResponseHeader=" << std::endl;
  m_ResponseHeader.debugDump(os, indent+2);

  if (mp_SessionObject)
  {
    ADebugDumpable::indent(os, indent+1) << "*mp_SessionObject=" << std::endl;
    mp_SessionObject->debugDump(os, indent+2);
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_SessionObject=NULL" << std::endl;
  
  //a_Event visitor
  ADebugDumpable::indent(os, indent+1) << "m_EventVisitor={" << std::endl;
  m_EventVisitor.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  //a_Output buffer
  ADebugDumpable::indent(os, indent+1) << "m_OutputBuffer={" << std::endl;
  m_OutputBuffer.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  //a_Context's local objects
  ADebugDumpable::indent(os, indent+1) << "m_ContextObjects={" << std::endl;
  m_ContextObjects.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_OutputXmlDocument={" << std::endl;
  m_OutputXmlDocument.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  //a_Flags
  ADebugDumpable::indent(os, indent+1) << "m_ConnectionFlags={" << std::endl;
  m_ConnectionFlags.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_ContextFlags={" << std::endl;
  m_ContextFlags.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_ContextTimer={" << std::endl;
  m_ContextTimer.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_RequestTimer={" << std::endl;
  m_RequestTimer.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_TimeoutTimer={" << std::endl;
  m_TimeoutTimer.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSContext::AOSContext(AFile_Socket *pFile, AOSServices& services) :
  m_ContextTimer(true),
  mp_SessionObject(NULL),
  m_Services(services),
  m_ContextObjects(CONTEXT),
  m_OutputXmlDocument(XML_ROOT),
  mp_Command(NULL),
  mp_DirConfig(NULL),
  mp_RequestFile(NULL),
  m_ConnectionFlags(AOSContext::CONFLAG_LAST),
  m_ContextFlags(AOSContext::CTXFLAG_LAST)
{
  reset(pFile);
}

AOSContext::~AOSContext()
{
  try
  {
    delete mp_RequestFile;  //a_Context owns the file object
  }
  catch(...) {}
}

void AOSContext::reset(AFile_Socket *pFile)
{
  if (!mp_RequestFile)
  {
    //a_New client connection
    m_EventVisitor.useLifespanTimer().start();
  }

  if (pFile)
  {
    delete mp_RequestFile;
    mp_RequestFile = pFile;
    
    ARope rope("AOSContext[",11);
    rope.append(mp_RequestFile->getSocketInfo().m_address);
    rope.append(':');
    rope.append(AString::fromInt(mp_RequestFile->getSocketInfo().m_port));
    rope.append(']');
    m_EventVisitor.useName().assign(rope);

    m_ConnectionFlags.clear();
  }

  m_RequestHeader.clear();
  m_ResponseHeader.clear();
  m_OutputBuffer.clear();
  m_OutputXmlDocument.clear();
  m_OutputXmlDocument.useRoot().useName().assign(XML_ROOT);
  mp_Command = NULL;

  m_ContextFlags.clear();
}

void AOSContext::finalize()
{
  //a_Close it if still open
  if (mp_RequestFile)
  {
    if (m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
    {
      setExecutionState(ASW("AOSContextManager::deallocate: Socket error detected",52));
    }
    else
    {
      setExecutionState(ASW("AOSContextManager::deallocate: Closing socket connection",56));
      mp_RequestFile->close();
    }
    
    pDelete(mp_RequestFile);
  }

  //a_Clear ouput model and buffer and stop event timer
  m_EventVisitor.set(ASW("Finalizing context.",19));
  m_OutputBuffer.clear(true);
  m_OutputXmlDocument.clear();
  m_EventVisitor.reset(true);
}

void AOSContext::clear()
{
  reset(NULL);
  m_EventVisitor.set(ASW("Clearing context.",17));
  m_EventVisitor.clear();
  m_ConnectionFlags.clear();
}

ABitArray& AOSContext::useConnectionFlags()
{
  return m_ConnectionFlags;
}

ABitArray& AOSContext::useContextFlags()
{
  return m_ContextFlags;
}

AOSContext::Status AOSContext::init()
{
  //a_Start the timer
  m_RequestTimer.start();

  //a_Reset context if pipelining, else it was already reset
  if (m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
  {
    //a_Reset if pipelining, if not this object woulod have been reset with a new socket prior to this call
    reset(NULL);
  }

  AOSContext::Status status = _processHttpHeader();
  if (AOSContext::STATUS_OK != status)
    return status;

  setExecutionState(ASW("AOSContext: Processing HTTP header",34), false);

  m_ContextFlags.clearBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT);
  m_ContextFlags.clearBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT);
  m_ContextFlags.clearBit(AOSContext::CTXFLAG_IS_REDIRECTING);

  m_RequestHeader.useUrl().setServer(m_Services.useConfiguration().getReportedHostname());
  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_HTTPS))
  {
    m_RequestHeader.useUrl().setProtocol(AUrl::HTTPS);
    m_RequestHeader.useUrl().setPort(m_Services.useConfiguration().getReportedHttpsPort());
  }
  else
  {
    m_RequestHeader.useUrl().setProtocol(AUrl::HTTP);
    m_RequestHeader.useUrl().setPort(m_Services.useConfiguration().getReportedHttpPort());
  }

  //a_Map response MIME type based on request extension
  setResponseMimeTypeFromRequestExtension();

  //a_Get associated directory config
  mp_DirConfig = m_Services.useConfiguration().getDirectoryConfig(m_RequestHeader.getUrl());

  //a_Set the command
  if (!setCommandFromRequestUrl())
  {
    m_EventVisitor.set(ARope("AOSContext::init: ")+ASWNL("Unable to find the command for request URL: ")+m_RequestHeader.useUrl());
  }

  //a_Initialize response header
  m_ResponseHeader.setPair(AHTTPHeader::HT_RES_Server, m_Services.useConfiguration().getReportedServer());

  return AOSContext::STATUS_OK;
}

AOSContext::Status AOSContext::_processHttpHeader()
{
  setExecutionState(ASW("AOSContext: Reading HTTP method",31), false, 30000.0);  //a_Read header for 60 seconds
  AString str(8188, 1024);

  // Sum( N * sleeptime, 0 to N-1)
  static int s_firstCharReadRetries = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/http/first-char-read-tries", 15);
  int tries = 0;
  int sleeptime = 20;
  char c = '\x0';
  size_t bytesRead = mp_RequestFile->read(c);
  if (0 == bytesRead && m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_AVAILABLE_PENDING))
  {
    //a_If select thinks there is data but we cannot read any then socket is dead
    if (!m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
    {
      m_EventVisitor.set(
        ASW("AOSContext: detected a closed socket",36), 
        false
      );
      
      return AOSContext::STATUS_HTTP_SOCKET_CLOSED;
    }
    else
      return AOSContext::STATUS_HTTP_INCOMPLETE_NODATA;
  }

  while (
    tries < s_firstCharReadRetries 
    && 1 != bytesRead
  )
  {
    ARope rope("AOSContext: Sleep cycle ",24);
    rope.append(AString::fromInt(tries));
    setExecutionState(rope);

    AThread::sleep(sleeptime);
    sleeptime += 20;
    ++tries;
    bytesRead = mp_RequestFile->read(c);
  }
  
  if (s_firstCharReadRetries <= tries)
  {
    ARope rope("AOSContext: Unable to read first char within ",43);
    rope.append(AString::fromInt(s_firstCharReadRetries));
    rope.append(" retries",8);
    m_EventVisitor.set(rope);
    return AOSContext::STATUS_HTTP_INCOMPLETE_NODATA;
  }

  //a_Add first letter
  str.append(c);

  bool needMoreData = false;
  bool methodRead = false;
  switch(c)
  {
    case 'G': // GET
    {
      if (2 != mp_RequestFile->read(str, 2))  // Read the ET
      {
        needMoreData = true;
        break;
      }

      if (
           'E' != str.at(1)
        || 'T' != str.at(2)
      )
        break;

      methodRead = true;
    }
    break;

    case 'P': // POST, PUT
    {
      if (1 != mp_RequestFile->read(c))   // Read O or U
      {
        needMoreData = true;
        break;
      }

      str.append(c);
      switch(c)
      {
        case 'O':  // POST
        {
          if (2 != mp_RequestFile->read(str, 2))  // Read ST
          {
            needMoreData = true;
            break;
          }

          if (
               'O' != str.at(1)
            || 'S' != str.at(2)
            || 'T' != str.at(3)
          )
            break;
        }
        break;

        case 'U':  // PUT
        {
          if (1 != mp_RequestFile->read(str, 1))  // Read T
          {
            needMoreData = true;
            break;
          }

          if (
               'U' != str.at(1)
            || 'T' != str.at(2)
          )
            break;
        }
        break;
      }

      methodRead = true;
    }
    break;

    case 'H': // HEAD
    {
      if (3 != mp_RequestFile->read(str, 3))  // Read the EAD
      {
        needMoreData = true;
        break;
      }

      if (
           'E' != str.at(1)
        || 'A' != str.at(2)
        || 'D' != str.at(3)
      )
        break;

      methodRead = true;
    }
    break;

    case 'D': // DELETE
    {
      if (5 != mp_RequestFile->read(str, 5))  // Read the ELETE
      {
        needMoreData = true;
        break;
      }

      if (
           'E' != str.at(1)
        || 'L' != str.at(2)
        || 'E' != str.at(3)
        || 'T' != str.at(4)
        || 'E' != str.at(5)
      )
        break;

      methodRead = true;
    }
    break;

    case 'O': // OPTIONS
    {
      if (6 != mp_RequestFile->read(str, 6))  // Read the PTIONS
      {
        needMoreData = true;
        break;
      }

      if (
           'P' != str.at(1)
        || 'T' != str.at(2)
        || 'I' != str.at(3)
        || 'O' != str.at(4)
        || 'N' != str.at(5)
        || 'S' != str.at(6)
      )
        break;

      methodRead = true;
    }
    break;

    case 'T': // TRACE
    {
      if (4 != mp_RequestFile->read(str, 4))  // Read the RACE
      {
        needMoreData = true;
        break;
      }

      if (
           'R' != str.at(1)
        || 'A' != str.at(2)
        || 'C' != str.at(3)
        || 'E' != str.at(4)
      )
        break;

      methodRead = true;
    }
    break;

    case 'C': // CONNECT
    {
      if (6 != mp_RequestFile->read(str, 6))  // Read the ONNECT
      {
        needMoreData = true;
        break;
      }

      if (
           'O' != str.at(1)
        || 'N' != str.at(2)
        || 'N' != str.at(3)
        || 'E' != str.at(4)
        || 'C' != str.at(5)
        || 'T' != str.at(6)
      )
        break;

      methodRead = true;
    }
    break;
  }
  
  if (needMoreData)
  {
    //a_Not enough data to read the method
    mp_RequestFile->putBack(str);

    m_EventVisitor.set(AString("AOSContext: Insufficiet data reading HTTP method: ",50)+str);
    return AOSContext::STATUS_HTTP_INCOMPLETE_METHOD;
  }

  //a_Read the space to make sure the method is valid
  if (methodRead && 1 == mp_RequestFile->read(c))
  {
    if (' ' != c)
    {
      ARope rope("AOSContext: Invalid char after \'", 32);
      rope.append(str);
      rope.append("\' method: \'", 11);
      rope.append(c);
      rope.append("\': ",3);
      rope.append(str);
      m_EventVisitor.set(rope, true);
      return AOSContext::STATUS_HTTP_INVALID_AFTER_METHOD_CHAR;
    }
    else
      str.append(' ');
  }

  //a_If method not read, read the socket data and log exception
  if (!methodRead)
  {
    ARope rope("AOSContext: Invalid first char: \\", 33);
    rope.append(AString::fromInt((int)c));
    rope.append(": ",2);
    ATextConverter::makeAsciiPrintable(str, rope);
    m_EventVisitor.set(rope, true);
    return AOSContext::STATUS_HTTP_INVALID_FIRST_CHAR;
  }

  setExecutionState(ASW("AOSContext: Reading HTTP line zero",34), false, 2000.0);
  if (AConstant::npos == (bytesRead = mp_RequestFile->readLine(str, 4096, false)))
  {
    m_EventVisitor.set(AString("Line zero incomplete: '",22)+str+'\'');

    //a_Put the stuff we read back
    mp_RequestFile->putBack(str);

    return AOSContext::STATUS_HTTP_INCOMPLETE_LINEZERO;
  }
  str.append(AConstant::ASTRING_CRLF);

  //a_Read until only 2 bytes (CR and LF) are read which signifies a blank line and end of http header
  setExecutionState(ASW("AOSContext: Reading HTTP header",31), false, 60000.0);  //a_Read header for 60 seconds
  size_t headerBytes = mp_RequestFile->readUntil(str, AConstant::ASTRING_CRLF, true, false);
  while (2 != headerBytes && AConstant::npos != headerBytes)
  {
    headerBytes = mp_RequestFile->readUntil(str, AConstant::ASTRING_CRLF, true, false);
  }
  
  //a_Check if header not done
  if (AConstant::npos == headerBytes || headerBytes > 2)
  {
    m_EventVisitor.set(ARope("Incomplete HTTP header or CRLF CRLF not found: '", 45)+str+'\'');
    
    //a_Put the stuff we read back
    mp_RequestFile->putBack(str);
    
    //a_Signal that CRLFCRLF is not found and we put data back into read buffer
    //a_select will check if more is available and we will reparse this again
    return AOSContext::STATUS_HTTP_INCOMPLETE_CRLFCRLF;
  }

  //a_Parse the header
  setExecutionState(ASW("AOSContext: Parsing HTTP header",31), false, 5000.0);
  m_RequestHeader.parse(str);

  //a_Check if valid HTTP version
  if (!m_RequestHeader.isValidVersion())
    return AOSContext::STATUS_HTTP_VERSION_NOT_SUPPORTED;

  //a_Check if valid HTTP method
  switch (m_RequestHeader.getMethodId())
  {
    case AHTTPRequestHeader::METHOD_ID_OPTIONS:
      setExecutionState(ASW("AOSContext: OPTIONS request",27), false);
      return AOSContext::STATUS_HTTP_METHOD_OPTIONS;

    case AHTTPRequestHeader::METHOD_ID_CONNECT:
      setExecutionState(ASW("AOSContext: CONNECT not allowed",31), true);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_DELETE:
      setExecutionState(ASW("AOSContext: DELETE not allowed",30), true);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_PUT:
      setExecutionState(ASW("AOSContext: PUT not allowed",27), true);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_TRACE:
      setExecutionState(ASW("AOSContext: TRACE not allowed",29), true);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_UNKNOWN:
      setExecutionState(ASW("AOSContext: Unknown method",26), true);
      return AOSContext::STATUS_HTTP_UNKNOWN_METHOD;
  }

  if (!m_RequestHeader.isValidPath())
  {
    setExecutionState(ASW("AOSContext: Invalid path",24), true);
    return AOSContext::STATUS_HTTP_INVALID_REQUEST_PATH;
  }

  if (!m_RequestHeader.isValidHttp())
  {
    setExecutionState(ASW("AOSContext: Invalid http header",31), true);
    return AOSContext::STATUS_HTTP_INVALID_HEADER;
  }

  static bool isHttpPipeliningEnabled = m_Services.useConfiguration().useConfigRoot().getBool("/config/server/http/http11-pipelining-enabled", true);
  if (
       isHttpPipeliningEnabled
    && m_RequestHeader.isHttpPipeliningEnabled()
  )
  {
    //a_HTTP pipelining turned on
    m_ConnectionFlags.setBit(AOSContext::CONFLAG_IS_HTTP11_PIPELINING);
  }
  
  ARope rope("AOSContext: HTTP request header\r\n",33);
  rope.append(m_RequestHeader);
  setExecutionState(rope);

  return AOSContext::STATUS_OK;
}

void AOSContext::emitXml(AXmlElement& target) const
{  
  AASSERT(this, !target.useName().isEmpty());

  target.useAttributes().insert(ASW("this",4), AString::fromPointer(this));
  target.useAttributes().insert(ASW("request_timer",13), AString::fromDouble(m_RequestTimer.getInterval()));
  target.useAttributes().insert(ASW("context_timer",13), AString::fromDouble(m_ContextTimer.getInterval()));

  m_EventVisitor.emitXml(target.addElement(ASW("Events",6)));
  m_Services.useGlobalObjects().emitXml(target.addElement(ASW("GlobalObjects",13)));
  m_ContextObjects.emitXml(target.addElement(ASW("ContextObjects",14)));
  m_RequestHeader.emitXml(target.addElement(ASW("RequestHeader",13)));
  m_ResponseHeader.emitXml(target.addElement(ASW("ResponseHeader",14)));
  m_ConnectionFlags.emitXml(target.addElement(ASW("ConnectionFlags",15)));
  m_ContextFlags.emitXml(target.addElement(ASW("ContextFlags",12)));

  if (mp_SessionObject)
    mp_SessionObject->emitXml(target.addElement(ASW("Session",7)));

  //a_Check if command exists, if not it could be static content
  if (mp_Command)
    mp_Command->emitXml(target.addElement(ASW("Command",7)));
  else
    target.addElement(ASW("Command",7), ASW("NULL",4));
}

AFile_Socket& AOSContext::useSocket()
{
  if (!mp_RequestFile)
    ATHROW(this, AException::InvalidObject);

  return *mp_RequestFile;
}

AString AOSContext::getInputCommand() const
{
  AASSERT(this, mp_Command);
  if (mp_Command)
    return mp_Command->getInputProcessorName();
  else
  {
    AString contentType;
    m_RequestHeader.getPairValue(AHTTPHeader::HT_ENT_Content_Type, contentType);
    contentType.truncateAt(';');
    return contentType;
  }
}

AString AOSContext::getOutputCommand() const
{
  AASSERT(this, mp_Command);
  if (mp_Command)
    return mp_Command->getOutputGeneratorName();
  else 
    return AConstant::ASTRING_EMPTY;
}

const AXmlElement& AOSContext::getInputParams() const
{
  if (!mp_Command)
    ATHROW(this, AException::DoesNotExist);

  return mp_Command->getInputParams();
}

const AXmlElement& AOSContext::getOutputParams() const
{
  if (!mp_Command)
    ATHROW(this, AException::DoesNotExist);

  return mp_Command->getOutputParams();
}

const AOSDirectoryConfig *AOSContext::getDirConfig() const
{
  return mp_DirConfig;
}

const AOSCommand *AOSContext::getCommand() const
{
  return mp_Command;
}

AOSServices& AOSContext::useServices()
{
  return m_Services;
}

void AOSContext::addError(
  const AString& where,
  const AString& what,
  bool addDebugDump // = false
)
{
  ARope rope;
  if (addDebugDump)
  {
    debugDumpToAOutputBuffer(rope, 1);
  }
  else
  {
    m_RequestHeader.getUrl().emit(rope, true);
  }
  m_Services.useLog().add(where, what, rope, ALog::CRITICAL_ERROR);
  m_EventVisitor.set(where+":"+what, true);
}

void AOSContext::addError(
  const AString& where,
  const AException& what,
  bool addDebugDump // = false
)
{
  addError(where, what.what(), addDebugDump);
}

void AOSContext::setResponseMimeTypeFromRequestExtension()
{
  AString str(64,16);
  AString& strExt = m_RequestHeader.useUrl().getExtension();
  if (m_Services.useConfiguration().getMimeTypeFromExt(strExt, str))
    m_ResponseHeader.setPair(AHTTPResponseHeader::HT_ENT_Content_Type, str);
}

AEventVisitor& AOSContext::useEventVisitor()
{
  return m_EventVisitor;
}

void AOSContext::setSessionObject(AOSSessionData *pObj)
{
  if (!pObj)
    ATHROW(this, AException::InvalidParameter);
  else
    mp_SessionObject = pObj;
}

AOSSessionData& AOSContext::useSessionData()
{
  if (mp_SessionObject)
  {
    return *mp_SessionObject;
  }
  else
  {
    addError(ASWNL("AOSContext::useSessionData"), ASWNL("Session does not exist."));
    ATHROW_EX(this, AException::InvalidState, ASWNL("Session data does not exist for this context, may be manually turned off in command."));
  }
}

AXmlDocument& AOSContext::useModelXmlDocument()
{
  return m_OutputXmlDocument;
}

AXmlElement& AOSContext::useModel()
{
  return m_OutputXmlDocument.useRoot();
}

ARope& AOSContext::useOutputBuffer()
{ 
  return m_OutputBuffer;
}

AHTTPRequestHeader& AOSContext::useRequestHeader()
{ 
  return m_RequestHeader;
}

ACookies& AOSContext::useRequestCookies()
{ 
  return m_RequestHeader.useCookies();
}

AUrl& AOSContext::useRequestUrl()
{ 
  return m_RequestHeader.useUrl();
}

AQueryString& AOSContext::useRequestParameterPairs()
{ 
  return m_RequestHeader.useUrl().useParameterPairs();
}

AHTTPResponseHeader& AOSContext::useResponseHeader()
{ 
  return m_ResponseHeader;
}

ACookies& AOSContext::useResponseCookies()
{ 
  return m_ResponseHeader.useCookies();
}

AObjectContainer& AOSContext::useContextObjects()
{ 
  return m_ContextObjects;
}

void AOSContext::setExecutionState(const AEmittable& state, bool isError, double maxTimeInState)
{
  m_EventVisitor.set(state, isError, maxTimeInState);
}

void AOSContext::resetExecutionState()
{
  m_EventVisitor.reset();
}

void AOSContext::setExecutionState(const AException& ex)
{
  m_EventVisitor.set(ex);
}

size_t AOSContext::getModuleNames(LIST_AString& names) const
{
  if (!mp_Command)
    ATHROW_EX(this, AException::DoesNotExist, ASWNL("Command does not exist"));

  size_t ret = 0;
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = mp_Command->getModules().get().begin();
  while (cit != mp_Command->getModules().get().end())
  {
    names.push_back((*cit)->getModuleClass());
    ++ret;
    ++cit;
  }
  return ret;
}

const AXmlElement& AOSContext::getModuleParams(const AString& moduleName) const
{
  if (!mp_Command)
    ATHROW_EX(this, AException::DoesNotExist, ASWNL("Command does not exist"));

  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = mp_Command->getModules().get().begin();
  while (cit != mp_Command->getModules().get().end())
  {
    if (moduleName.equals((*cit)->getModuleClass()))
      return (*cit)->getParams();

    ++cit;
  }
  ATHROW_EX(this, AException::DoesNotExist, ARope("Module does not exist: ")+moduleName);
}

const ATimer& AOSContext::getContextTimer() const
{
  return m_ContextTimer;
}

const ATimer& AOSContext::getRequestTimer() const
{
  return m_RequestTimer;
}

ATimer& AOSContext::useTimeoutTimer()
{
  return m_TimeoutTimer;
}

void AOSContext::writeResponseHeader()
{
  AASSERT(this, mp_RequestFile);

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT))
    ATHROW_EX(this, AException::ProgrammingError, ASWNL("Response header has already been sent"));

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
    ATHROW_EX(this, AException::ProgrammingError, ASWNL("Output has already been sent"));

  m_EventVisitor.set(ASW("Sending HTTP response header",28));
  m_ResponseHeader.emit(*mp_RequestFile);
  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT);
  m_EventVisitor.reset();
}

void AOSContext::writeOutputBuffer(bool forceXmlDocument)
{
  if (forceXmlDocument)
  {
    writeOutputBuffer(ARope());
  }
  else
  {
    writeOutputBuffer(m_OutputBuffer);
  }
}

void AOSContext::writeOutputBuffer(const ARope& output)
{
  AASSERT(this, mp_RequestFile);

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
    ATHROW_EX(this, AException::ProgrammingError, ASWNL("Output has already been sent"));

  //a_If output buffer is empty then emit output XML document into it
  //a_This allows override of XML emit by manually adding data to the output buffer in output generator
  if (output.isEmpty())
  {
    AASSERT(this, !m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT));
    
    ARope fallbackOutput;
    m_OutputXmlDocument.emit(fallbackOutput);

    //a_Reset the content type to XML and add content-length
    m_ResponseHeader.setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml", 8));
    m_ResponseHeader.setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(fallbackOutput.getSize()));

    writeResponseHeader();

    m_EventVisitor.set(ASW("Sending XML document",20));
    fallbackOutput.emit(*mp_RequestFile);
    m_EventVisitor.reset();
  }
  else
  {
    if (!m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT))
      writeResponseHeader();
    
    m_EventVisitor.set(ASW("Sending output",14));
    output.emit(*mp_RequestFile);
    m_EventVisitor.reset();
  }

  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT);
}

int AOSContext::getDumpContextLevel() const
{
  AString str;
  if (
    m_Services.useConfiguration().isDumpContextAllowed() &&
    m_RequestHeader.getUrl().getParameterPairs().get(ASW("dumpContext", 11), str)
  )
  {
    if (str.equals(ASW("on",2)))
      return 1;       //a_on is used in checkboxes and equals level 1
    else
      return str.toInt();
  }
  return 0;
}

void AOSContext::dumpContext(int dumpContextLevel)
{
  if (dumpContextLevel > 0)
  {
    AXmlElement& eDumpContext = m_OutputXmlDocument.useRoot().addElement(ASW("dumpContext",11));
    AXmlElement& eContext = eDumpContext.addElement(ASW("context",7));
    switch(dumpContextLevel)
    {
      case 2:
        eContext.addElement(ASW("buffer",6)).addData(m_OutputBuffer, AXmlElement::ENC_CDATAHEXDUMP);
        eContext.addElement(ASW("debugDump",9)).addData(*this, AXmlElement::ENC_CDATADIRECT);
        m_Services.useConfiguration().getConfigRoot().emitXml(
          eDumpContext.addElement(ASW("configuration",13))
        );
      case 1:
        emitXml(eContext);
      default:
        if (m_EventVisitor.getErrorCount() > 0)
        {
          //a_Process and display error as XML
          m_EventVisitor.emitXml(m_OutputXmlDocument.useRoot().addElement(ASW("error", 5)));
        }
    }
  }
}

bool AOSContext::setCommandFromRequestUrl()
{
  //a_Get the new command
  mp_Command = m_Services.useConfiguration().getCommand(m_RequestHeader.getUrl());
  if (!mp_Command)
    return false;

  //a_Check if this is an alias
  if (!mp_Command->getCommandAlias().isEmpty())
  {
    setExecutionState(ARope("Aliased: ",9)+mp_Command->getCommandAlias());
    mp_Command = m_Services.useConfiguration().getCommand(mp_Command->getCommandAlias());
    if (!mp_Command->getCommandAlias().isEmpty())
    {
      setExecutionState(ASWNL("Aliased command is also an alias, which is not supported."));
      mp_Command = NULL;
    }

    if (!mp_Command)
      return false;
  }

  //a_AJAX style output (terse)
  if (mp_Command->isForceAjax() || m_RequestHeader.useUrl().useParameterPairs().exists(ASW("ajax",4)))
    m_ContextFlags.setBit(CTXFLAG_IS_AJAX);

  return true;
}

bool AOSContext::setNewCommandPath(const AString& commandpath)
{
  m_RequestHeader.useUrl().setPathAndFilename(commandpath);
  return setCommandFromRequestUrl();
}

bool AOSContext::isOutputCommitted() const
{
  return m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT);
}

void AOSContext::setOutputCommitted(bool b)
{
  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT, b);
}

void AOSContext::persistSession()
{
  m_Services.useSessionManager().persistSession(mp_SessionObject);
}

void AOSContext::setResponseRedirect(const AString& url)
{
  m_ResponseHeader.setStatusCode(AHTTPResponseHeader::SC_302_Moved_Temporarily);
  m_ResponseHeader.setPair(AHTTPResponseHeader::HT_RES_Location, url);
}
