#ifndef INCLUDED__AContext_HPP__
#define INCLUDED__AContext_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AFile_Socket.hpp"
#include "AStringHashMap.hpp"
#include "AObjectContainer.hpp"
#include "ARope.hpp"
#include "AEventVisitor.hpp"
#include "ATimer.hpp"
#include "AXmlEmittable.hpp"
#include "AOSSessionData.hpp"
#include "AXmlDocument.hpp"
#include "AbitArray.hpp"

class AUrl;
class AQueryString;
class ACookies;
class AOSServices;
class AOSCommand;
class AThread;
class AOSDirectoryConfig;

class AOS_BASE_API AOSContext : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  ctor
  */
  AOSContext(AFile_Socket *, AOSServices&);

  /*!
  dtor
  */
  virtual ~AOSContext();

  /*!
  Resets the context and assigns a new socket, deleting previous
  If file is NULL, only the context is cleared while file is kept (used in pipelining)
  */
  void reset(AFile_Socket *pFile);

  /*!
  Init status codes
  */
  enum Status
  {
    STATUS_OK = 0,
    STATUS_HTTP_INCOMPLETE_NODATA,
    STATUS_HTTP_INCOMPLETE_METHOD,
    STATUS_HTTP_INCOMPLETE_LINEZERO,
    STATUS_HTTP_INCOMPLETE_CRLFCRLF,
    STATUS_HTTP_INVALID_AFTER_METHOD_CHAR,
    STATUS_HTTP_INVALID_FIRST_CHAR,
    STATUS_HTTP_VERSION_NOT_SUPPORTED,
    STATUS_HTTP_METHOD_NOT_ALLOWED,
    STATUS_HTTP_METHOD_OPTIONS,
    STATUS_HTTP_UNKNOWN_METHOD,
    STATUS_HTTP_UNKNOWN_VERSION,
    STATUS_HTTP_INVALID_REQUEST_PATH,
    STATUS_HTTP_INVALID_HEADER,
    STATUS_HTTP_SOCKET_CLOSED
  };

  /*!
  Initialize the context
  With pipelining not everything is reset
  Returns status of the init (AOSContext::STATUS_OK if all is well and HTTP header has been read and processed)
  */
  AOSContext::Status init();

  /*!
  Write response header and set response header sent flag
  */
  void writeResponseHeader();

  /*!
  Writes output buffer
  If it is empty it emits the output XML document into in and sets content type to text/xml
  If response header was not yet written, it will write it
  forceXmlDocument - uses XML document even if output buffer has data
  */
  void writeOutputBuffer(bool forceXmlDocument = false);

  /*!
  Same as writeOutputBuffer, except the AEmittable is written instead of the internal output buffer
  Internal buffer is all-purpose and should always be used, exception is when the output is a result of internal buffer
    like compression or variation
  */
  void writeOutputBuffer(const ARope&);

  /*!
  Output XML document's root node, appended to output buffer after all modules execute
  This is where the output goes and majority of modules will modify the root element
  If the output buffer is empty, this document is emitted as XML
  */
  AXmlElement& useOutputRootXmlElement();    //a_Shortcut to useOutputXmlDocument().useRoot()

  /*!
  The output XML document (owner of the root xml element)
  A document holds header instructions and root xml element
  */
  AXmlDocument& useOutputXmlDocument();

  /*!
  Output buffer, sent to output after output stage
  If this is empty then XML document is the result
  Putting data here will override the XML document, so you have to sent MIME type as needed
  */
  ARope& useOutputBuffer();

  /*!
  Set execution state of the context
  */
  void setExecutionState(const AString&, bool isError = false, double maxTimeInState = INVALID_TIME_INTERVAL);
  void setExecutionState(const AException&);
  
  /*!
  True if the current state max time is exceeded (timeout set by setExecutionState)
  */
  bool isStateTimedOut() const;

  /*!
  Access to the socket associated with this context
  This is provided for flexibility but XML document should be the prefered output
  Output buffer should be used when output is not XML
  If the output is written to socket manually please set the appropriate ContentFlag types (such as header sent, output sent, etc)
  Please use this with caution, it's here to make your life easy in some odd edge case, but be careful
  */
  AFile_Socket& useSocket();
    
  /*!
  Request HTTP header and helpers
  */
  AHTTPRequestHeader& useRequestHeader();
  ACookies& useRequestCookies();
  AUrl& useRequestUrl();
  AQueryString& useRequestParameterPairs();

  /*!
  Response HTTP header and helpers
  */
  AHTTPResponseHeader& useResponseHeader();
  ACookies& useResponseCookies();
  
  /*!
  Uses request header to determine which MIME type the response header will use
  */
  void setResponseMimeTypeFromRequestExtension();

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  Context specific data used to contain data for the duration of the request
  Also usable as inter-module communication
  */
  AObjectContainer& useContextObjects();

  /*!
  Gets/sets the session data object/data
  */
  void setSessionObject(AOSSessionData *);
  AStringHashMap& useSessionData();

  /*!
  Error message handling
  Calling this will include context dump in log
  addDebugDump will force a debugDump on this context and add it to the log (can be long and mainly for critical errors)
  */
  void addError(const AString& where, const AString& what, bool addDebugDump = false);
  void addError(const AString& where, const AException& what, bool addDebugDump = false);
  AEventVisitor& useEventVisitor();
    
  /*!
  Directrory config object (NULL if does not exist)
  */
  const AOSDirectoryConfig *getDirConfig() const;

  /*!
  Command object (NULL if does not exist)
  */
  const AOSCommand *getCommand() const;

  /*!
  Input Processor command (usually Content-Type of the HTTP header)
  */
  AString getInputCommand() const;

  /*!
  Modules and params
  */
  size_t getModuleNames(LIST_AString&) const;
  const AXmlElement& getModuleParams(const AString& moduleName) const;
  
  /*!
  Input and output params
  */
  const AXmlElement& getInputParams() const;
  const AXmlElement& getOutputParams() const;

  /*!
  Output type
  */
  AString getOutputCommand() const;

  /*!
  Gets the context timer (lifetime of the context)
  Can span several requests if pipelining is enabled
  */
  const ATimer& getContextTimer() const;

  /*!
  Gets the request timer
  */
  const ATimer& getRequestTimer() const;

  /*!
  Timeout timer (used for socket reading/keepalive/etc)
  */
  ATimer& useTimeoutTimer();

  /*!
  Services and request
  */
  AOSServices& useServices();

  /*!
  Connection flags
  */
  enum ConnectionFlag
  {
    CONFLAG_IS_AVAILABLE_SELECTED = 0, //a_IsAvailable queue did a select with no success
    CONFLAG_IS_AVAILABLE_PENDING,      //a_IsAvailable queue did a select and either data is pending or socket is closed
    CONFLAG_IS_HTTP11_PIPELINING,      //a_HTTP 1.1 pipelining is being used
    CONFLAG_IS_SOCKET_ERROR,           //a_Socket in error state
    //
    CONFLAG_LAST                       //a_Last one used for sizing the bit array
  };

  /*!
  Access to connection based flags
  */
  ABitArray& useConnectionFlags();

  /*!
  Context flags
  */
  enum ContextFlag
  {
    CTXFLAG_IS_AJAX = 0,              //a_Request is AJAX, use minimal XML
    CTXFLAG_IS_HTTPS,                 //a_Request from HTTPS socket listener
    CTXFLAG_IS_RESPONSE_HEADER_SENT,  //a_Response header already written
    CTXFLAG_IS_OUTPUT_SENT,           //a_Output as been written already (response header sent)
    //
    CTXFLAG_LAST                      //a_Last one used for sizing the bit array
  };

  /*!
  Access to context specific flags
  */
  ABitArray& useContextFlags();

  /*!
  Output constants
  */
  static const AString CONTEXT;
  static const AString XML_ROOT;

protected:
  //a_Read and parse Http header
  AOSContext::Status _processHttpHeader();

  //a_Event visitor
  AEventVisitor m_EventVisitor;
  
  //a_HTTP data
  AHTTPRequestHeader m_RequestHeader;
  AHTTPResponseHeader m_ResponseHeader;

  //a_Output buffer
  ARope m_OutputBuffer;

  //a_Context objects
  AObjectContainer m_ContextObjects;

  //a_Output document
  AXmlDocument m_OutputXmlDocument;

  //a_Configuration
  AOSServices& m_Services;

  //a_Associated session if any
  AOSSessionData *mp_SessionObject;

  /*!
  Command of this context (cached to prevent constant lookup in AOSConfiguration)
  */
  const AOSCommand *mp_Command;
  
  /*!
  Directory config of this context (cached to prevent constant lookup in AOSConfiguration)
  */
  const AOSDirectoryConfig *mp_DirConfig;

  /*!
  See ConnectionFlag
  */
  ABitArray m_ConnectionFlags;

  /*!
  See ContextFlag
  */
  ABitArray m_ContextFlags;
  
  /*!
  Context timer
  */
  ATimer m_ContextTimer;

  /*!
  Request timer
  */
  ATimer m_RequestTimer;

  /*!
  Timeout timer used in context processing
  */
  ATimer m_TimeoutTimer;

private:  
  //a_No copying
  AOSContext& operator =(const AOSContext&) { return *this; }

  //a_Socket associated with context
  AFile_Socket *mp_RequestFile;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AContext_HPP__
