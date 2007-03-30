#include "pchAOS_Base.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AOSCommand.hpp"
#include "ALock.hpp"
#include "AThread.hpp"
#include "AOSConfiguration.hpp"

const AString AOSContext::CONTEXT("context");
const AString AOSContext::XML_ROOT("root");
const AString AOSContext::XML_DOCUMENT("xml_document");

#ifdef __DEBUG_DUMP__
void AOSContext::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSContext @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_EventVisitor=" << std::endl;
  m_EventVisitor.debugDump(os, indent+2);
  
  //a_Input file
  ADebugDumpable::indent(os, indent+1) << "*mp_File=" << std::endl;
  if (mp_RequestFile)
    mp_RequestFile->debugDump(os, indent+2);

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

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSContext::AOSContext(AFile_Socket *pFile, AOSServices& services) :
  m_ContextTimer(true),
  mp_SessionObject(NULL),
  m_Services(services),
  m_ContextObjects(CONTEXT),
  m_OutputXmlDocument(XML_ROOT),
  mp_Command(NULL),
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
  if (pFile)
  {
    delete mp_RequestFile;
    mp_RequestFile = pFile;
    
    ARope rope("AOSContext[",11);
    rope.append(mp_RequestFile->getAddress());
    rope.append(':');
    rope.append(AString::fromInt(mp_RequestFile->getPort()));
    rope.append(']');
    m_EventVisitor.useName().assign(rope);

    m_ConnectionFlags.clear();
  }

  m_RequestHeader.clear();
  m_ResponseHeader.clear();
  m_OutputBuffer.clear();
  m_OutputXmlDocument.clear();
  mp_Command = NULL;

  m_ContextFlags.clear();
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
  if (m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
  {
    //a_Reset if pipelining, if not this object woulod have been reset with a new socket prior to this call
    reset(NULL);
  }

  AOSContext::Status status = _processHttpHeader();
  if (AOSContext::STATUS_OK != status)
    return status;

  setExecutionState(ASW("AOSContext: Processing HTTP header",34), false);

  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT, false);
  m_ContextFlags.setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT, false);

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

  //a_Get the new command
  mp_Command = m_Services.useConfiguration().getCommand(m_RequestHeader.getUrl());

  //a_AJAX style output (terse)
  if (
       (mp_Command && mp_Command->isForceAjax())
    || m_RequestHeader.useUrl().useParameterPairs().exists(ASW("ajax",4))
  )
  {
    m_ContextFlags.setBit(CTXFLAG_IS_AJAX);
  }

  //a_Initialize response header
  m_ResponseHeader.setPair(AHTTPHeader::HT_RES_Server, m_Services.useConfiguration().getReportedServer());

  return AOSContext::STATUS_OK;
}

AOSContext::Status AOSContext::_processHttpHeader()
{
  setExecutionState(ASW("AOSContext: Reading HTTP method",31), false, 30000.0);  //a_Read header for 60 seconds
  AString str(8188, 1024);

  // 20 + 40 + 60 + 80 + 100 + 120 + 140 + 160 + 180 + 200 +220 + 240 = 1560ms
  int tries = 0;
  int sleeptime = 20;
  char c = '\x0';
  size_t bytesRead = mp_RequestFile->read(c);
  if (0 == bytesRead && m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_AVAILABLE_PENDING))
  {
    //a_If select things there is data but we cannot read any then socket is dead
    m_EventVisitor.set(
      ASW("AOSContext: detected a closed socket",36), 
      !m_ConnectionFlags.isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING)
    );
    return AOSContext::STATUS_HTTP_SOCKET_CLOSED;
  }

  while (
    tries < m_Services.useConfiguration().getHttpFirstCharReadTries() 
    && 1 != bytesRead
  )
  {
    AThread::sleep(sleeptime);
    sleeptime += 20;
    ++tries;
    bytesRead = mp_RequestFile->read(c);
  }
  
  if (m_Services.useConfiguration().getHttpFirstCharReadTries() <= tries)
  {
    ARope rope("AOSContext: Unable to read first char within ",43);
    rope.append(AString::fromInt(m_Services.useConfiguration().getHttpFirstCharReadTries()));
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

  setExecutionState(ASW("AOSContext: Reading HTTP line zero",34), false, 2000.0);  //a_Read header for 60 seconds
  if (AConstant::npos == (bytesRead = mp_RequestFile->readLine(str, 4096, false)))
  {
    m_EventVisitor.set(AString("Line zero incomplete: '",22)+str+'\'');

    //a_Put the stuff we read back
    mp_RequestFile->putBack(str);

    return AOSContext::STATUS_HTTP_INCOMPLETE_LINEZERO;
  }
  str.append(AString::sstr_CRLF);

  setExecutionState(ASW("AOSContext: Reading HTTP header",31), false, 6000.0);  //a_Read header for 60 seconds
  if (AConstant::npos == mp_RequestFile->readUntil(str, AString::sstr_CRLFCRLF, true, false))
  {
    m_EventVisitor.set(ARope("Incomplete HTTP header, CRLF CRLF not found: '", 45)+str+'\'');
    
    //a_Put the stuff we read back
    mp_RequestFile->putBack(str);
    
    //a_Signal that CRLFCRLF is not found and we put data back into read buffer
    //a_select will check if more is available and we will reparse this again
    return AOSContext::STATUS_HTTP_INCOMPLETE_CRLFCRLF;
  }

  //a_Parse the header
  setExecutionState(ASW("AOSContext: Parsing HTTP header",31), false, 5000.0);
  m_RequestHeader.parse(str);

  if (
       m_Services.useConfiguration().isHttpPipeliningEnabled()
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

void AOSContext::emit(AXmlElement& target) const
{  
  AASSERT(this, !target.useName().isEmpty());

  target.useAttributes().insert(ASW("this",4), AString::fromPointer(this));
  target.useAttributes().insert(ASW("timer",5), AString::fromDouble(m_ContextTimer.getInterval()));

  m_EventVisitor.emit(target.addElement(ASW("Events",6)));
  m_Services.useGlobalObjects().emit(target.addElement(ASW("GlobalObjects",13)));
  m_ContextObjects.emit(target.addElement(ASW("ContextObjects",14)));
  m_RequestHeader.emit(target.addElement(ASW("RequestHeader",13)));
  m_ResponseHeader.emit(target.addElement(ASW("ResponseHeader",14)));
  m_ConnectionFlags.emit(target.addElement(ASW("ConnectionFlags",15)));
  m_ContextFlags.emit(target.addElement(ASW("ContextFlags",12)));

  if (mp_SessionObject)
    mp_SessionObject->emit(target.addElement(ASW("Session",7)));

  //a_Check if command exists, if not it could be static content
  if (mp_Command)
    mp_Command->emit(target.addElement(ASW("Command",7)));
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
    m_RequestHeader.find(AHTTPHeader::HT_ENT_Content_Type, contentType);
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
    return AString::sstr_Empty;
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
#ifdef __DEBUG_DUMP__
  if (addDebugDump)
  {
    debugDumpToAOutputBuffer(rope, 1);
  }
  else
#endif
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

AStringHashMap& AOSContext::useSessionData()
{
  if (mp_SessionObject)
  {
    return mp_SessionObject->useData();
  }
  else
  {
    addError("AOSContext::useSessionObjects", "Session does not exist, falling back on context data.");
    ATHROW_EX(this, AException::InvalidState, "Session object does not exist");
  }
}

AXmlDocument& AOSContext::useOutputXmlDocument()
{
  return m_OutputXmlDocument;
}

AXmlElement& AOSContext::useOutputRootXmlElement()
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

void AOSContext::setExecutionState(const AString& state, bool isError, double maxTimeInState)
{
  m_EventVisitor.set(state, isError, maxTimeInState);
}

void AOSContext::setExecutionState(const AException& ex)
{
  m_EventVisitor.set(ex);
}

size_t AOSContext::getModuleNames(LIST_AString& names) const
{
  if (!mp_Command)
    ATHROW(this, AException::DoesNotExist);

  size_t ret = 0;
  AOSCommand::MODULES::const_iterator cit = mp_Command->getModuleInfoContainer().begin();
  while (cit != mp_Command->getModuleInfoContainer().end())
  {
    names.push_back((*cit).m_Name);
    ++ret;
    ++cit;
  }
  return ret;
}

const AXmlElement& AOSContext::getModuleParams(const AString& moduleName) const
{
  if (!mp_Command)
    ATHROW_EX(this, AException::DoesNotExist, ARope("Command does not exist"));

  AOSCommand::MODULES::const_iterator cit = mp_Command->getModuleInfoContainer().begin();
  while (cit != mp_Command->getModuleInfoContainer().end())
  {
    if (moduleName.equals((*cit).m_Name))
      return (*cit).m_ModuleParams;

    ++cit;
  }
  ATHROW_EX(this, AException::DoesNotExist, ARope("Module does not exist: ")+moduleName);
}

const ATimer& AOSContext::getContextTimer() const
{
  return m_ContextTimer;
}

ATimer& AOSContext::useTimeoutTimer()
{
  return m_TimeoutTimer;
}

void AOSContext::writeResponseHeader()
{
  AASSERT(this, mp_RequestFile);

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT))
    ATHROW_EX(this, AException::ProgrammingError, "Response header has already been sent");

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
    ATHROW_EX(this, AException::ProgrammingError, "Output has already been sent");

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
    writeOutputBuffer(m_OutputBuffer);
}

void AOSContext::writeOutputBuffer(const ARope& output)
{
  AASSERT(this, mp_RequestFile);

  if (m_ContextFlags.isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
    ATHROW_EX(this, AException::ProgrammingError, "Output has already been sent");

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
