/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AOSController.hpp"
#include "AThread.hpp"
#include "AOSConfiguration.hpp"
#include "ASocketException.hpp"
#include "AZlib.hpp"

extern "C" int luaopen_aos(lua_State *L);

const AString AOSContext::XML_ROOT("root");
const AString AOSContext::OBJECTNAME("__AOSContext__");

const AString AOSContext::S_REQUEST("REQUEST",7);
const AString AOSContext::S_RESPONSE("RESPONSE",8);
const AString AOSContext::S_SESSION("SESSION",7);
const AString AOSContext::S_OUTPUT("OUTPUT",6);
const AString AOSContext::S_GLOBAL("GLOBAL",6);
const AString AOSContext::S_ERROR("ERROR",5);
const AString AOSContext::S_MESSAGE("MESSAGE",7);
const AString AOSContext::S_CONTEXT("CONTEXT",7);
const AString AOSContext::S_MODEL("MODEL",5);
const AString AOSContext::S_REFERER("REFERER",7);

const AString AOSContext::S_GZIP("gzip",4);

void AOSContext::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_EventVisitor=" << std::endl;
  m_EventVisitor.debugDump(os, indent+2);
  
  //a_Input file
  if (mp_RequestFile)
  {
    ADebugDumpable::indent(os, indent+1) << "*mp_RequestFile={" << std::endl;
    mp_RequestFile->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "*mp_RequestFile=NULL" << std::endl;

  //a_Associated directory config
  if (mp_DirConfig)
  {
    ADebugDumpable::indent(os, indent+1) << "m_DirConfig=" << std::endl;
    mp_DirConfig->debugDump(os, indent+2);
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_DirConfig=NULL" << std::endl;

  //a_Associated command
  if (mp_Controller)
  {
    ADebugDumpable::indent(os, indent+1) << "m_Controller=" << std::endl;
    mp_Controller->debugDump(os, indent+2);
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_Controller=NULL" << std::endl;

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
  m_OutputBuffer(10240),
  m_OutputXmlDocument(XML_ROOT),
  mp_Controller(NULL),
  mp_DirConfig(NULL),
  mp_RequestFile(NULL),
  m_ConnectionFlags(AOSContext::CONFLAG_LAST),
  m_ContextFlags(AOSContext::CTXFLAG_LAST),
  m_EventVisitor(ASW("AOSContext:",11), AEventVisitor::EL_DEBUG),
  mp_LuaTemplateContext(NULL),
  m_DefaultLuaLibraries(ALuaEmbed::LUALIB_ALL_SAFE)
{
  m_EventVisitor.useName().append(AString::fromPointer(this));
  
  if (pFile)
    reset(pFile);
}

AOSContext::~AOSContext()
{
  try
  {
    delete mp_RequestFile;
    delete mp_LuaTemplateContext;
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
    
    if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
    {
      m_EventVisitor.startEvent(ASWNL("AOSContext::reset: Replacing file"), AEventVisitor::EL_DEBUG);
    }

    m_ConnectionFlags.clear();
  }

  pDelete(mp_LuaTemplateContext);

  setSessionObject(NULL);

  m_RequestHeader.clear();
  m_ResponseHeader.clear();
  m_OutputBuffer.clear();
  m_OutputXmlDocument.clear(XML_ROOT);
  mp_Controller = NULL;
  mp_DirConfig = NULL;

  m_ContextObjects.clear();
  m_ContextFlags.clear();

  if (mp_RequestFile && m_EventVisitor.isLoggingDebug())
  {
    ARope rope("AOSContext::reset[",18);
    rope.append(mp_RequestFile->getSocketInfo().m_address);
    rope.append(':');
    rope.append(AString::fromInt(mp_RequestFile->getSocketInfo().m_port));
    rope.append(']');
    m_EventVisitor.startEvent(rope, AEventVisitor::EL_DEBUG);
  }
}

void AOSContext::finalize(bool releaseMemory)
{
  m_EventVisitor.startEvent(ASW("Finalizing the context",22), AEventVisitor::EL_EVENT);

  //a_Close it if still open
  if (mp_RequestFile)
  {
    if (m_ConnectionFlags.isClear(AOSContext::CONFLAG_IS_SOCKET_CLOSED))
    {
      m_EventVisitor.startEvent(ASW("AOSContextManager::deallocate: Closing socket connection",56));
      mp_RequestFile->close();
    }
    
    pDelete(mp_RequestFile);
  }

  //a_Release lua context (not going to be rendering any more templates
  if (mp_LuaTemplateContext)
  {
    pDelete(mp_LuaTemplateContext);
  }

  //a_Finalize the session data
  setSessionObject(NULL);

  //a_Clear objects, buffers and model
  m_OutputBuffer.clear();
  m_OutputXmlDocument.clear(XML_ROOT);
  m_ContextObjects.clear();

  if (releaseMemory)
  {
    m_EventVisitor.clear();
    m_RequestHeader.clear();
    m_ResponseHeader.clear();
  }
  else
  {
    //a_Stop the event
    m_EventVisitor.endEvent();
    m_EventVisitor.useLifespanTimer().stop();
  }
}

void AOSContext::clear()
{
  reset(NULL);
  m_EventVisitor.clear();
  m_ConnectionFlags.clear();
  pDelete(mp_RequestFile);
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

  try
  {
    AOSContext::Status status = _processHttpHeader();
    if (AOSContext::STATUS_OK != status)
      return status;
  }
  catch(ASocketException& ex)
  {
    ARope rope("AOSContext: Socket exception: ",30);
    rope.append(AString::fromU4(ex.getErrno()));
    m_EventVisitor.startEvent(rope, AEventVisitor::EL_ERROR);
    m_ConnectionFlags.setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
    if (mp_RequestFile)
      mp_RequestFile->close();
    return AOSContext::STATUS_HTTP_SOCKET_CLOSED;
  }

  //a_Add Date: to the response
  ATime timeNow;
  AString str;
  timeNow.emitRFCtime(str);
  m_ResponseHeader.set(AHTTPHeader::HT_GEN_Date, str);

  //a_Set session id on response if not part of request
  m_Services.useSessionManager().addSessionId(*this);
  
  //a_Set the hostname from either Hostname: request header or use config if not found or HTTP/1.0
  str.clear();
  if (m_RequestHeader.get(AHTTPHeader::HT_REQ_Host, str))
    m_RequestHeader.useUrl().setServer(str);
  else
    m_RequestHeader.useUrl().setServer(m_Services.useConfiguration().getReportedHostname());
  
  //a_HTTP/HTTPS
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
  m_Services.useConfiguration().setMimeTypeFromRequestExt(*this);

  //a_Get associated directory config
  mp_DirConfig = m_Services.useConfiguration().getDirectoryConfig(m_RequestHeader.getUrl());

  //a_Set the command
  if (!setControllerFromRequestUrl())
  {
    if (m_EventVisitor.isLoggingInfo())
    {
      ARope rope("AOSContext::init: Unable to find the command for request URL: ");
      rope.append(m_RequestHeader.useUrl());
      m_EventVisitor.startEvent(rope, AEventVisitor::EL_INFO);
    }
  }

  //a_Initialize response header
  m_ResponseHeader.set(AHTTPHeader::HT_RES_Server, m_Services.useConfiguration().getReportedServer());
			
  //a_Create session data since it is required
  if (mp_Controller && mp_Controller->isSessionRequired())
  {
    m_Services.useSessionManager().initOrCreateSession(*this);
  }

  str.clear();
  if (getSessionDataContent(AOSSessionData::LOCALE, str))
    setLocaleOnRequestHeader(str);

  return AOSContext::STATUS_OK;
}

void AOSContext::setLocaleOnRequestHeader(const AString& locale)
{
  //a_Set locale on the request header
  m_RequestHeader.remove(AHTTPHeader::HT_REQ_Accept_Language);
  if (locale.isEmpty())
  {
    if (m_EventVisitor.isLoggingDebug())
    {
      m_EventVisitor.startEvent(ASWNL("AOSContext::init: Removing request header locale"), AEventVisitor::EL_DEBUG);
    }
  }
  else
  {
    m_RequestHeader.set(AHTTPHeader::HT_REQ_Accept_Language, locale);
    if (m_EventVisitor.isLoggingDebug())
    {
      ARope rope("AOSContext::init: Changing the request header locale to session override:  Accept-Language: ");
      rope.append(locale);
      m_EventVisitor.startEvent(rope, AEventVisitor::EL_DEBUG);
    }
  }
}

bool AOSContext::_waitForFirstChar()
{
  int sleeptime = AOSConfiguration::SLEEP_STARTTIME;
  int tries = 0;
  char c;
  while (tries < AOSConfiguration::FIRST_CHAR_RETRIES)
  {
    if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
    {
      ARope rope("AOSContext: Sleep cycle ");
      rope.append(AString::fromInt(tries));
      m_EventVisitor.startEvent(rope, AEventVisitor::EL_DEBUG);
    }

    AThread::sleep(sleeptime);
    sleeptime += AOSConfiguration::SLEEP_INCREMENT;
    ++tries;
    size_t ret = mp_RequestFile->peek(c);
    switch(ret)
    {
      case AConstant::npos:
        return false;

      case AConstant::unavail:
      case 0:
        break;       // No data available yet, keep waiting

      default:
        return true; // Read something
    }
  }

  return false;
}

AOSContext::Status AOSContext::_processHttpHeader()
{
  if (m_EventVisitor.isLoggingInfo())
    m_EventVisitor.startEvent(ASW("AOSContext::INIT: Processing HTTP header",40), AEventVisitor::EL_INFO);
  
  if (!mp_RequestFile)
    ATHROW(this, AException::InvalidObject);

  m_EventVisitor.startEvent(ASW("AOSContext: Reading HTTP method",31), AEventVisitor::EL_INFO);
  AString str(8188, 1024);

  // Sum( N * sleeptime, 0 to N-1)
  char c = '\x0';
  size_t bytesRead = 0;
  
  bytesRead = mp_RequestFile->read(c);

  switch (bytesRead)
  {
    case AConstant::unavail:
    {
      //a_Data not available, try and read-wait to get it
      if (!_waitForFirstChar())
      {
        m_EventVisitor.startEvent(ASW("AOSContext: Unable to read first char after retries",51), AEventVisitor::EL_WARN);
        return AOSContext::STATUS_HTTP_INCOMPLETE_NODATA;
      }
      else
      {
        bytesRead = mp_RequestFile->read(c);
      }
    }
    break;

    case AConstant::npos:
      m_EventVisitor.startEvent(ASW("AOSContext: Error reading first char",36), AEventVisitor::EL_ERROR);
      return AOSContext::STATUS_HTTP_INCOMPLETE_NODATA;

    case 0:
      if (m_ConnectionFlags.isSet(AOSContext::CONFLAG_ISAVAILABLE_PENDING))
      {
        //a_0 bytes read yet isAvailable flagged it as having data which implies closed socket
        //a_Should not come here, isAvailable will handle closed sockets
        //a_If select thinks there is data but we cannot read any then socket is dead
        m_EventVisitor.startEvent(ASWNL("AOSContext: Handling closed socket (should be in isAvailable)"));
        if (mp_RequestFile)
          mp_RequestFile->close();
        return AOSContext::STATUS_HTTP_SOCKET_CLOSED;
      }
      else
      {
        //a_No data read wait and try to get more
        if (!_waitForFirstChar())
        {
          m_EventVisitor.startEvent(ASW("AOSContext: Zero data read looking for first char",49), AEventVisitor::EL_DEBUG);
          return AOSContext::STATUS_HTTP_INCOMPLETE_NODATA;
        }
        else
        {
          if (bytesRead = mp_RequestFile->read(c) > 0)
            m_ConnectionFlags.clearBit(AOSContext::CONFLAG_ISAVAILABLE_PENDING);
          else
          {
            //a_Something wrong here, peek says data is ready, read can't get it
            ATHROW(this, AException::ProgrammingError);
          }
        }
      }
    break;

    default:
      //a_Data was read, clear the pending flag set by isAvailable
      m_ConnectionFlags.clearBit(AOSContext::CONFLAG_ISAVAILABLE_PENDING);
    break;
  }
  AASSERT(this, 1 == bytesRead);

  //a_Start request timer once we have first character
  m_RequestTimer.start();

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

    AString strEvent("AOSContext: Insufficiet data reading HTTP method: ",50);
    strEvent.append(str);
    m_EventVisitor.startEvent(strEvent, AEventVisitor::EL_INFO);

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
      m_EventVisitor.startEvent(rope, AEventVisitor::EL_ERROR);
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
    m_EventVisitor.startEvent(rope, AEventVisitor::EL_ERROR);
    return AOSContext::STATUS_HTTP_INVALID_FIRST_CHAR;
  }

  if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
    m_EventVisitor.startEvent(ASW("AOSContext: Reading HTTP line zero",34), AEventVisitor::EL_DEBUG);
  if (AConstant::npos == (bytesRead = mp_RequestFile->readLine(str, 4096, false)))
  {
    {
      AString strEvent("Line zero incomplete: '",23);
      strEvent.append(str);
      strEvent.append('\'');
      m_EventVisitor.startEvent(strEvent);
    }

    //a_Put the stuff we read back
    mp_RequestFile->putBack(str);

    return AOSContext::STATUS_HTTP_INCOMPLETE_LINEZERO;
  }
  str.append(AConstant::ASTRING_CRLF);

  //a_Read until only 2 bytes (CR and LF) are read which signifies a blank line and end of http header
  if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
    m_EventVisitor.startEvent(ASW("AOSContext: Reading HTTP header",31), AEventVisitor::EL_INFO);
  
  size_t headerBytes = mp_RequestFile->readUntil(str, AConstant::ASTRING_CRLF, true, false);
  while (2 != headerBytes && AConstant::npos != headerBytes)
  {
    headerBytes = mp_RequestFile->readUntil(str, AConstant::ASTRING_CRLF, true, false);
  }
  
  //a_Check if header not done
  if (AConstant::npos == headerBytes || headerBytes > 2)
  {
    {
      ARope rope("Incomplete HTTP header or CRLF CRLF not found: '", 48);
      rope.append(str);
      rope.append('\'');
      m_EventVisitor.startEvent(rope);
    }

    //a_Put the stuff we read back
    mp_RequestFile->putBack(str);
    
    //a_Signal that CRLFCRLF is not found and we put data back into read buffer
    //a_select will check if more is available and we will reparse this again
    return AOSContext::STATUS_HTTP_INCOMPLETE_CRLFCRLF;
  }

  //a_Reset context if pipelining, else this is a new or already reset context (from freestore)
  if (m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
  {
    if (m_EventVisitor.isLoggingDebug())
      m_EventVisitor.startEvent(ASWNL("AOSContext: Resetting context before parse"), AEventVisitor::EL_DEBUG);

    //a_If not this object would have been reset with a new socket prior to this call
    reset(NULL);
  }
  else
  {
    if (m_EventVisitor.isLoggingDebug())
      m_EventVisitor.startEvent(ASWNL("AOSContext: Clearing context flags before parse"), AEventVisitor::EL_DEBUG);

    //a_Reset context for the current request
    m_ContextFlags.clearBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT);
    m_ContextFlags.clearBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT);
    m_ContextFlags.clearBit(AOSContext::CTXFLAG_IS_REDIRECTING);
    m_ContextFlags.clearBit(AOSContext::CTXFLAG_IS_CACHE_CONTROL_NO_CACHE);
  }

  //a_Parse the header
  m_EventVisitor.startEvent(ASW("AOSContext: Parsing HTTP header",31), AEventVisitor::EL_EVENT, 5000.0);
  m_RequestHeader.parse(str);

  return _postProcessHttpHeader();
}

AOSContext::Status AOSContext::_postProcessHttpHeader()
{
  //a_Check if valid HTTP version
  if (!m_RequestHeader.isValidVersion())
    return AOSContext::STATUS_HTTP_VERSION_NOT_SUPPORTED;

  //a_Check if valid HTTP method
  switch (m_RequestHeader.getMethodId())
  {
    case AHTTPRequestHeader::METHOD_ID_OPTIONS:
      m_EventVisitor.startEvent(ASW("AOSContext: OPTIONS request",27));
      return AOSContext::STATUS_HTTP_METHOD_OPTIONS;

    case AHTTPRequestHeader::METHOD_ID_CONNECT:
      m_EventVisitor.startEvent(ASW("AOSContext: CONNECT not allowed",31), AEventVisitor::EL_ERROR);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_DELETE:
      m_EventVisitor.startEvent(ASW("AOSContext: DELETE not allowed",30), AEventVisitor::EL_ERROR);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_PUT:
      m_EventVisitor.startEvent(ASW("AOSContext: PUT not allowed",27), AEventVisitor::EL_ERROR);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_TRACE:
      m_EventVisitor.startEvent(ASW("AOSContext: TRACE not allowed",29), AEventVisitor::EL_ERROR);
      return AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED;

    case AHTTPRequestHeader::METHOD_ID_UNKNOWN:
      m_EventVisitor.startEvent(ASW("AOSContext: Unknown method",26), AEventVisitor::EL_ERROR);
      return AOSContext::STATUS_HTTP_UNKNOWN_METHOD;
  }

  if (!m_RequestHeader.isValidPath())
  {
    m_EventVisitor.startEvent(ASW("AOSContext: Invalid path",24), AEventVisitor::EL_ERROR);
    return AOSContext::STATUS_HTTP_INVALID_REQUEST_PATH;
  }

  if (!m_RequestHeader.isValidHttp())
  {
    m_EventVisitor.startEvent(ASW("AOSContext: Invalid http header",31), AEventVisitor::EL_ERROR);
    return AOSContext::STATUS_HTTP_INVALID_HEADER;
  }

  //a_Check if to keep the connection alive
  if (
    AOSConfiguration::IS_HTTP_PIPELINING_ENABLED                                                 // user enabled
    && m_RequestHeader.isHttpPipeliningEnabled()                               // request header allows it 
    && AHTTPRequestHeader::METHOD_ID_PUT != m_RequestHeader.getMethodId()      // method is not POST
  )
  {
    //a_HTTP pipelining turned on
    m_ConnectionFlags.setBit(AOSContext::CONFLAG_IS_HTTP11_PIPELINING);
    m_ResponseHeader.set(AHTTPHeader::HT_GEN_Connection, ASW("keep-alive",10));
    //TODO: do we really need this? m_ResponseHeader.setPair(AHTTPHeader::HT_GEN_Keep_Alive, ASW("timeout=15, max=100",19));
    if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
      m_EventVisitor.startEvent(ASW("Connection: keep-alive set on response header",45), AEventVisitor::EL_DEBUG);
  }
  else
  {
    //a_Connection close
    m_ConnectionFlags.clearBit(AOSContext::CONFLAG_IS_HTTP11_PIPELINING);
    m_ResponseHeader.set(AHTTPHeader::HT_GEN_Connection, ASW("close",5));
    if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
      m_EventVisitor.startEvent(ASW("Connection: Close set on response header",40), AEventVisitor::EL_DEBUG);
  }

  if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
  {
    ARope rope("AOSContext: HTTP request header\r\n",33);
    rope.append(m_RequestHeader);
    m_EventVisitor.startEvent(rope, AEventVisitor::EL_INFO);
  }

  return AOSContext::STATUS_OK;
}

AXmlElement& AOSContext::emitXml(AXmlElement& thisRoot) const
{  
  AASSERT(this, !thisRoot.useName().isEmpty());

  thisRoot.useAttributes().insert(ASW("this",4), AString::fromPointer(this));
  thisRoot.useAttributes().insert(ASW("request_timer",13), AString::fromDouble(m_RequestTimer.getInterval()));
  thisRoot.useAttributes().insert(ASW("context_timer",13), AString::fromDouble(m_ContextTimer.getInterval()));

  m_EventVisitor.emitXml(thisRoot.addElement(ASW("EventVisitor",12)));
  m_Services.useGlobalObjects().emitXml(thisRoot.addElement(ASW("GlobalObjects",13)));
  m_ContextObjects.emitXml(thisRoot.addElement(ASW("ContextObjects",14)));
  m_RequestHeader.emitXml(thisRoot.addElement(S_REQUEST));
  m_ResponseHeader.emitXml(thisRoot.addElement(S_RESPONSE));
  m_ConnectionFlags.emitXml(thisRoot.addElement(ASW("ConnectionFlags",15)));
  m_ContextFlags.emitXml(thisRoot.addElement(ASW("ContextFlags",12)));

  if (mp_SessionObject)
    mp_SessionObject->emitXml(thisRoot.addElement(S_SESSION));

  //a_Check if command exists, if not it could be static content
  if (mp_Controller)
    mp_Controller->emitXml(thisRoot.addElement(AOSController::S_CONTROLLER));
  else
    thisRoot.addElement(AOSController::S_CONTROLLER, AConstant::ASTRING_NULL);

  //a_Add globals
  m_Services.useGlobalObjects().emitXml(thisRoot.addElement(S_GLOBAL));

  return thisRoot;
}

AFile_Socket& AOSContext::useSocket()
{
  if (!mp_RequestFile)
    ATHROW(this, AException::InvalidObject);

  return *mp_RequestFile;
}

AString AOSContext::getInputCommand() const
{
  AASSERT(this, mp_Controller);
  if (mp_Controller)
    return mp_Controller->getInputProcessorName();
  else
  {
    AString contentType;
    m_RequestHeader.get(AHTTPHeader::HT_ENT_Content_Type, contentType);
    contentType.truncateAt(';');
    return contentType;
  }
}

AString AOSContext::getOutputCommand() const
{
  AASSERT(this, mp_Controller);
  if (mp_Controller)
    return mp_Controller->getOutputGeneratorName();
  else 
    return AConstant::ASTRING_EMPTY;
}

const AXmlElement& AOSContext::getInputParams() const
{
  if (!mp_Controller)
    ATHROW(this, AException::DoesNotExist);

  return mp_Controller->getInputParams();
}

const AXmlElement& AOSContext::getOutputParams() const
{
  if (!mp_Controller)
    ATHROW(this, AException::DoesNotExist);

  return mp_Controller->getOutputParams();
}

const AOSDirectoryConfig *AOSContext::getDirConfig() const
{
  return mp_DirConfig;
}

const AOSController *AOSContext::getController() const
{
  return mp_Controller;
}

AOSServices& AOSContext::useServices()
{
  return m_Services;
}

void AOSContext::addError(
  const AString& where,
  const AEmittable& what,
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
  m_EventVisitor.startEvent(where+":"+what, AEventVisitor::EL_ERROR);
  m_OutputXmlDocument.useRoot().addElement(S_ERROR).addData(what);
}

AEventVisitor& AOSContext::useEventVisitor()
{
  return m_EventVisitor;
}

AOSSessionData *AOSContext::setSessionObject(AOSSessionData *pObj)
{
  AOSSessionData *pRet = mp_SessionObject;
  mp_SessionObject = pObj;
  if (mp_SessionObject)
  {
    m_ContextFlags.setBit(CTXFLAG_IS_USING_SESSION_DATA);
    mp_SessionObject->init(*this);
  }
  else
  {
    m_ContextFlags.clearBit(CTXFLAG_IS_USING_SESSION_DATA);
    if (pRet)
      pRet->finalize(*this);
  }

  return pRet;
}

bool AOSContext::existsSessionData() const
{
  if (mp_SessionObject)
  {
    // Session object exists
    return true;
  }
  else
  {
    //a_See if session id exists
    AString sessionId;
    if (m_RequestHeader.getCookies().getValue(AOSSessionManager::SESSIONID, sessionId))
    {
      return m_Services.useSessionManager().exists(sessionId);
    }
    else
      return false;  // id doesn't exist in cookies so session doesn't exist
  }
}

bool AOSContext::getSessionDataContent(const AString& path, AString& value)
{
  if (!mp_SessionObject)
  {
    //a_See if session id exists
    AString sessionId;
    if (m_RequestHeader.getCookies().getValue(AOSSessionManager::SESSIONID, sessionId))
    {
      if (m_Services.useSessionManager().exists(sessionId))
      {
        ALock lock(useSessionData().useSyncObject());
        return useSessionData().useData().emitContentFromPath(path, value);
      }
    }
  }
  else
  {
    // Session object exists already
    ALock lock(useSessionData().useSyncObject());
    return useSessionData().useData().emitContentFromPath(path, value);
  }
  return false;
}

AOSSessionData& AOSContext::useSessionData()
{
  if (!mp_SessionObject)
  {
    //a_Lazy create
    m_Services.useSessionManager().initOrCreateSession(*this);
  }
  
  if (mp_SessionObject)
  {
    AASSERT(this, m_ContextFlags.isSet(CTXFLAG_IS_USING_SESSION_DATA));
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

AOutputBuffer& AOSContext::useOutputBuffer()
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

ABasePtrContainer& AOSContext::useContextObjects()
{ 
  return m_ContextObjects;
}

size_t AOSContext::getModuleNames(LIST_AString& names) const
{
  if (!mp_Controller)
    ATHROW_EX(this, AException::DoesNotExist, ASWNL("Controller does not exist"));

  size_t ret = 0;
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = mp_Controller->getModules().get().begin();
  while (cit != mp_Controller->getModules().get().end())
  {
    names.push_back((*cit)->getModuleClass());
    ++ret;
    ++cit;
  }
  return ret;
}

const AXmlElement& AOSContext::getModuleParams(const AString& moduleName) const
{
  if (!mp_Controller)
    ATHROW_EX(this, AException::DoesNotExist, ASWNL("Controller does not exist"));

  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = mp_Controller->getModules().get().begin();
  while (cit != mp_Controller->getModules().get().end())
  {
    if (moduleName.equals((*cit)->getModuleClass()))
      return (*cit)->getParams();

    ++cit;
  }
  
  ARope rope("Module does not exist: ",23);
  rope.append(moduleName);
  ATHROW_EX(this, AException::DoesNotExist, rope);
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

size_t AOSContext::writeResponseHeader()
{
  AASSERT(this, mp_RequestFile);

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT))
    ATHROW_EX(this, AException::ProgrammingError, ASWNL("Response header has already been sent"));

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
    ATHROW_EX(this, AException::ProgrammingError, ASWNL("Output has already been sent"));

  //a_Turn off caching on dynamic pages
  if ((mp_Controller && !mp_Controller->isCacheControlNoCache()) || m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_CACHE_CONTROL_NO_CACHE))
  {
    if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
      m_EventVisitor.startEvent(ASWNL("Cache-Control: no-cache set on response"), AEventVisitor::EL_DEBUG);

    m_ResponseHeader.set(AHTTPResponseHeader::HT_GEN_Cache_Control, ASW("no-cache",8));
  }
  else
  {
    if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
      m_EventVisitor.startEvent(ASWNL("Response is cacheable"), AEventVisitor::EL_DEBUG);
  }

  m_EventVisitor.startEvent(ASW("Sending HTTP response header",28));

  AString str(10240, 4096);

  //a_Add some common response header pairs
  ATime timeNow;
  timeNow.emitRFCtime(str);
  m_ResponseHeader.set(AHTTPHeader::HT_GEN_Date, str);

  str.clear();
  m_ResponseHeader.emit(str);
  size_t written = _write(str, str.getSize());

  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT);

  if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
  {
    m_EventVisitor.addEvent(str, AEventVisitor::EL_INFO);

    AString str("Header bytes written: ",21);
    str.append(AString::fromSize_t(written));

    m_EventVisitor.startEvent(str, AEventVisitor::EL_INFO);
  }
  return written;
}

void AOSContext::writeOutputBuffer(bool forceXmlDocument)
{
  m_EventVisitor.startEvent(ASW("AOSContext: write output",24), AEventVisitor::EL_INFO);
  AASSERT(this, mp_RequestFile);
  size_t headerWritten = 0, bodyWritten = 0;
  int gzipLevel = _calculateGZipLevel(m_OutputBuffer.getSize());
  if (gzipLevel > 0 && gzipLevel < 10 && !forceXmlDocument)
  {
    AASSERT_EX(this, m_ContextFlags.isClear(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT), ASWNL("Response header already written, incompatible with gzip output"));
    if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
      m_EventVisitor.startEvent(ASW("Compressing",11), AEventVisitor::EL_INFO);

    AString original(m_OutputBuffer);
    AString compressed(m_OutputBuffer.getSize()+16, 1024);
    AZlib::gzipDeflate(original, compressed, gzipLevel);

    //TODO: Add compressed to the cache

    m_ResponseHeader.set(AHTTPHeader::HT_ENT_Content_Encoding, ASW("gzip",4));
    m_ResponseHeader.set(AHTTPHeader::HT_RES_Vary, ASW("Accept-Encoding",15));
    m_ResponseHeader.set(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(compressed.getSize()));

    //a_The writing of the output
    headerWritten = writeResponseHeader();
    m_EventVisitor.startEvent(ASW("AOSContext: Writing compressed",30));
    
    size_t originalSize = compressed.getSize();
    bodyWritten = _write(compressed, compressed.getSize());
    AASSERT(this, bodyWritten == originalSize);
  }
  else
  {
    if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
      ATHROW_EX(this, AException::ProgrammingError, ASWNL("Output has already been sent"));

    //a_This allows override of XML emit by manually adding data to the output buffer in output generator
    if (forceXmlDocument)
    {
      m_OutputXmlDocument.useRoot().addElement(S_OUTPUT).addData(m_OutputBuffer, AXmlElement::ENC_CDATAHEXDUMP);
      m_OutputBuffer.clear();
      if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
      {
        m_EventVisitor.startEvent(ASW("AOSContext: XML document forced, clearing current output and emitting model",75), AEventVisitor::EL_DEBUG);
      }
    }
    
    //a_If output buffer is empty then emit output XML document into it unless we are redirecting, then we don't
    if (m_OutputBuffer.isEmpty() && m_ContextFlags.isClear(AOSContext::CTXFLAG_IS_REDIRECTING))
    {
      m_OutputXmlDocument.emit(m_OutputBuffer);
      m_Services.useConfiguration().setMimeTypeFromExt(ASW("xml",3), *this);
      m_ResponseHeader.setStatusCode(AHTTPResponseHeader::SC_200_Ok);
      if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
      {
        m_EventVisitor.startEvent(ASW("AOSContext: Empty output and no redirect, using model XML as document and setting response to 200",97), AEventVisitor::EL_DEBUG);
      }
    }

    //a_Write header if not already written, if it was already written, we assume it is correct
    if (m_ContextFlags.isClear(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT))
    {
      m_ResponseHeader.set(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(m_OutputBuffer.getSize()));
      headerWritten = writeResponseHeader();  //a_Sets flag that it was sent
    }

    m_EventVisitor.startEvent(ASW("AOSContext: Writing uncompressed",32));
    size_t originalSize = m_OutputBuffer.getSize();
    bodyWritten = _write(m_OutputBuffer, m_OutputBuffer.getSize());
    AASSERT(this, bodyWritten == originalSize);
  }

  //a_Log info about bytes written
  if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
  {
    AString str("Total bytes written: ",21);
    str.append(AString::fromSize_t(headerWritten+bodyWritten));
    str.append('(');
    str.append(AString::fromSize_t(headerWritten));
    str.append('+');
    str.append(AString::fromSize_t(bodyWritten));
    str.append(')');

    m_EventVisitor.startEvent(str, AEventVisitor::EL_INFO);
  }

  m_EventVisitor.endEvent();
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
        eContext.addElement(ASW("debugDump",9)).addData(*this, AXmlElement::ENC_CDATASAFE);
        m_Services.useConfiguration().getConfigRoot().emitXml(eDumpContext.addElement(ASW("configuration",13)));
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

bool AOSContext::setControllerFromRequestUrl()
{
  //a_Get the new command
  mp_Controller = m_Services.useConfiguration().getController(m_RequestHeader.getUrl());
  if (!mp_Controller)
    return false;

  //a_Controller based execution disables the browser cache unless something overrides it
  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_CACHE_CONTROL_NO_CACHE);

  //a_Check if this is an alias
  const AString& alias = mp_Controller->getAlias();
  if (!alias.isEmpty())
  {
    {
      ARope str("Aliased: ",9);
      str.append(mp_Controller->getAlias());
      m_EventVisitor.startEvent(str);
    }

    if ('/' == alias.at(0))
    {
      //a_Abosulte path
      mp_Controller = m_Services.useConfiguration().getController(mp_Controller->getAlias());
    }
    else
    {
      //a_Relative path, get base and add alias
      AString str;
      mp_Controller->emitBasePath(str);
      str.append(alias);
      
      {
        AString strEvent("Relative aliased to: ");
        strEvent.append(str);
        m_EventVisitor.startEvent(strEvent);
      }
      
      mp_Controller = m_Services.useConfiguration().getController(str);
    }

    if (!mp_Controller->getAlias().isEmpty())
    {
      m_EventVisitor.startEvent(ASWNL("Aliased command is also an alias, which is not supported."));
      mp_Controller = NULL;
    }

    if (!mp_Controller)
      return false;
  }

  //a_AJAX style output (terse)
  if (mp_Controller->isForceAjax() || m_RequestHeader.useUrl().useParameterPairs().exists(ASW("ajax",4)))
    m_ContextFlags.setBit(CTXFLAG_IS_AJAX);

  return true;
}

bool AOSContext::setNewControllerPath(const AString& commandpath)
{
  m_RequestHeader.useUrl().setPathAndFilename(commandpath);
  return setControllerFromRequestUrl();
}

void AOSContext::persistSession()
{
  m_Services.useSessionManager().persistSession(mp_SessionObject);
}

void AOSContext::setResponseRedirect(const AString& url)
{
  m_ResponseHeader.setStatusCode(AHTTPResponseHeader::SC_302_Moved_Temporarily);
  m_ResponseHeader.set(AHTTPResponseHeader::HT_RES_Location, url);
  m_ResponseHeader.set(AHTTPResponseHeader::HT_ENT_Content_Length, AConstant::ASTRING_ZERO);
  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_REDIRECTING);
  m_OutputBuffer.clear();
}

int AOSContext::_calculateGZipLevel(size_t documentSize)
{
  if (AOSConfiguration::GZIP_IS_ENABLED)
  {
    //a_User override
    AString strDeflateLevel;
    if (m_RequestHeader.useUrl().useParameterPairs().get(S_GZIP, strDeflateLevel))
      return strDeflateLevel.toInt();
    
    //a_Controller definition
    if (mp_Controller && mp_Controller->getGZipLevel() > 0)
      return mp_Controller->getGZipLevel();
    
    //a_Match extension and minimum size
    const SET_AString& exts = m_Services.useConfiguration().getCompressedExtensions();
    if (
         exts.end() != exts.find(m_RequestHeader.getUrl().getExtension())
         && documentSize >= AOSConfiguration::GZIP_MIN_SIZE
    )
    {
      //a_Check if the client can accept gzip
      AString strAcceptEncoding;
      if (
           m_RequestHeader.get(AHTTPHeader::HT_REQ_Accept_Encoding, strAcceptEncoding)
        && AConstant::npos != strAcceptEncoding.findNoCase(S_GZIP)
      )
      return AOSConfiguration::GZIP_DEFAULT_LEVEL;
    }
  }

  return 0;
}

bool AOSContext::isOutputBufferEmpty() const
{
  return m_OutputBuffer.isEmpty();
}

void AOSContext::clearOutputBuffer()
{
  m_OutputBuffer.clear();
}

size_t AOSContext::getOutputBufferSize() const
{
  return m_OutputBuffer.getSize();
}

bool AOSContext::processStaticPage()
{
  if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
    m_EventVisitor.startEvent(ASW("AOSContext::processStaticPage: Serving static content",53), AEventVisitor::EL_INFO);
  
  AFilename httpFilename;
  AString filepart(m_RequestHeader.useUrl().getPathAndFilename());
  if (filepart.isEmpty() 
      || filepart.equals(AConstant::ASTRING_SLASH)
      || m_RequestHeader.useUrl().getFilename().isEmpty()
  )
  {
    httpFilename.join(filepart, true);
    httpFilename.useFilename().assign(m_Services.useConfiguration().getAosDefaultFilename());
  }
  else
  {
    httpFilename.join(m_RequestHeader.useUrl().getPathAndFilename(), false);
  }
  
  if (!httpFilename.isValid())
  {
    //a_Invalid filename
    m_ResponseHeader.setStatusCode(AHTTPResponseHeader::SC_400_Bad_Request);
    return false;
  }
  
  int dumpContextLevel = getDumpContextLevel();

  ACache_FileSystem::HANDLE pFile(NULL, false);
  ATime modified;
  ACacheInterface::STATUS fileStatus = m_Services.useCacheManager().getStaticFile(*this, httpFilename, pFile, modified);
  switch (fileStatus)
  {
    case ACache_FileSystem::NOT_FOUND:
    {
      //a_Handle file not found
      ARope rope("AOSContext::processStaticPage: File not found (HTTP-404 static): ",65);
      rope.append(httpFilename);
      m_EventVisitor.startEvent(rope);
      
      //a_Set response status and return with failed (display error template)
      m_ResponseHeader.setStatusCode(AHTTPResponseHeader::SC_404_Not_Found);
    }
    return false;

    case ACache_FileSystem::FOUND_NOT_MODIFIED:
    {
      ARope rope("AOSContext::processStaticPage: File not modified (HTTP-304): ",61);
      rope.append(httpFilename);
      m_EventVisitor.startEvent(rope);
      
      //a_Set status 304 and return true (no need to display error template)
      m_ResponseHeader.setStatusCode(AHTTPResponseHeader::SC_304_Not_Modified);
      writeResponseHeader();
    }
    return true;

    case ACache_FileSystem::FOUND_NOT_CACHED:
    case ACache_FileSystem::FOUND:
    {
      size_t bytesToSend = pFile->getSize();
      int gzipLevel = _calculateGZipLevel(bytesToSend);
      if (!dumpContextLevel && !gzipLevel)
      {
        //a_Context not dumped and no compression is needed
        AASSERT(this, !pFile.isNull());
        
        //a_Set modified date
        m_ResponseHeader.setLastModified(modified);
        m_ResponseHeader.set(AHTTPResponseHeader::HT_ENT_Content_Length, AString::fromS8(bytesToSend));
        
        AString ext;
        httpFilename.emitExtension(ext);
        m_Services.useConfiguration().setMimeTypeFromExt(ext, *this);
        writeResponseHeader();

        //a_Stream content
        {
          ARope rope("AOSContext::processStaticPage: Streaming file: ",47);
          rope.append(httpFilename);
          m_EventVisitor.startEvent(rope);
        }

        AASSERT(this, pFile);
        AASSERT(this, pFile->isNotEof());
        size_t bytesWritten = _write(*pFile, pFile->getSize());

        if (AConstant::npos == bytesWritten)
          m_EventVisitor.addEvent(ASW("AOSContext::processStaticPage: Failed to write",46));
        else
          m_EventVisitor.addEvent(ARope("AOSContext::processStaticPage: Streamed bytes: ",47)+AString::fromSize_t(bytesWritten), AEventVisitor::EL_INFO);

        if (bytesWritten != bytesToSend)
        {
          AString str("AOSContext::processStaticPage: Failed to write complete data: ");
          str.append(AString::fromSize_t(bytesWritten));
          str.append('/');
          str.append(AString::fromSize_t(bytesToSend));
          m_EventVisitor.addEvent(str, AEventVisitor::EL_ERROR);
        }
        
        m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT);
        return true;
      }
      else
      {
        //a_Buffer the file
        AASSERT(this, !pFile.isNull());

        //a_Set modified date
        m_ResponseHeader.setLastModified(modified);

        //a_Buffer the file, Content-Length set in the write header
        m_EventVisitor.startEvent(ARope("AOSContext::processStaticPage: Sending file: ",45)+httpFilename);
        pFile->emit(m_OutputBuffer);
        m_EventVisitor.addEvent(ARope("AOSContext::processStaticPage: Output buffer bytes: ",52)+AString::fromSize_t(m_OutputBuffer.getSize()), AEventVisitor::EL_INFO);
      }
    }
    break;

    default:
      AASSERT(this, false);  //a_Unknown return type?
  }
  
  //a_Check if context is being dumped
  bool forceXml = false;
  if (dumpContextLevel > 0)
  {
    m_EventVisitor.startEvent(ASW("AOSContext::processStaticPage: Dumping context",46), AEventVisitor::EL_INFO);
    dumpContext(dumpContextLevel);
    forceXml = true;
  }

  //a_Write
  try
  {
    m_EventVisitor.startEvent(ASW("AOSContext::processStaticPage: Writing output buffer",52), AEventVisitor::EL_DEBUG);
    writeOutputBuffer(forceXml);
  }
  catch(ASocketException& ex)
  {
    m_ConnectionFlags.setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
    if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
      m_EventVisitor.addEvent(ex, AEventVisitor::EL_INFO);
    return false;
  }
  
  m_EventVisitor.endEvent();
  return true;
}

size_t AOSContext::_write(ARandomAccessBuffer& data, size_t originalSize)
{
  AASSERT(this, mp_RequestFile);
  size_t bytesToWrite = originalSize;
  size_t bytesWritten = 0;
  size_t index = 0;
  int sleepRetries = 0;
  while (bytesToWrite)
  {
    size_t ret = data.access(*mp_RequestFile, index, bytesToWrite);
    switch(ret)
    {
      //a_Finished writing or EOF on read
      case 0:
      {
        if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
        {
          AString str("AOSContext: flush returned 0, written bytes so far: ");
          str.append(AString::fromSize_t(bytesWritten));
          m_EventVisitor.addEvent(str, AEventVisitor::EL_DEBUG);
        }
      }
      return (bytesWritten > 0 ? bytesWritten : ret);

      case AConstant::npos:
      {
        if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
        {
          AString str("AOSContext: flush returned npos, written bytes so far: ");
          str.append(AString::fromSize_t(bytesWritten));
          m_EventVisitor.addEvent(str, AEventVisitor::EL_DEBUG);
        }
      }
      return (bytesWritten > 0 ? bytesWritten : ret);

      //a_Would block
      case AConstant::unavail:
      {
        if (m_EventVisitor.isLogging(AEventVisitor::EL_DEBUG))
        {
          AString str("AOSContext: flush returned unavail, sleeping and retrying, written bytes so far: ");
          str.append(AString::fromSize_t(bytesWritten));
          m_EventVisitor.addEvent(str, AEventVisitor::EL_DEBUG);
        }
        if (sleepRetries < AOSConfiguration::UNAVAILABLE_RETRIES)
        {
          ++sleepRetries;
          AThread::sleep(AOSConfiguration::UNAVAILABLE_SLEEP_TIME);
        }
        else
          return (bytesWritten > 0 ? bytesWritten : ret);
      }
      break;
        
      default:
        bytesToWrite -= ret;
        bytesWritten += ret;
        index += ret;
        sleepRetries = 0;
      break;
    }
  }

  AASSERT(this, bytesWritten == originalSize);
  if (m_EventVisitor.isLogging(AEventVisitor::EL_INFO))
  {
    AString str("AOSContext::_write bytes: ",26);
    str.append(AString::fromSize_t(bytesWritten));
    m_EventVisitor.addEvent(str, AEventVisitor::EL_INFO);
  }
  return bytesWritten;
}

bool AOSContext::isConnectionClose() const
{
  const AString close("close", 5);
  bool is = m_RequestHeader.equalsNoCase(AHTTPHeader::HT_GEN_Connection, close);
  is |= m_ResponseHeader.equalsNoCase(AHTTPHeader::HT_GEN_Connection, close);
  return is;
}

ALuaTemplateContext& AOSContext::useLuaTemplateContext()
{
  if (!mp_LuaTemplateContext)
  {
    mp_LuaTemplateContext = new ALuaTemplateContext(m_ContextObjects, m_OutputXmlDocument, m_EventVisitor, m_DefaultLuaLibraries);
    mp_LuaTemplateContext->addUserDefinedLibrary(luaopen_aos);  //a_Add AOS function library loader

    //a_Reference self from inside the objects
    AASSERT(this, NULL == mp_LuaTemplateContext->useObjects().get(OBJECTNAME));
    mp_LuaTemplateContext->useObjects().insert(OBJECTNAME, this, false);
  }
  return *mp_LuaTemplateContext;
}
