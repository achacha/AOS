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
  Return codes used during execution
  */
  enum ReturnCode
  {
    RETURN_ERROR = 0,
    RETURN_OK = 1,
    RETURN_REDIRECT
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
  Root element of the XML model used for this request
  Ouput generator will use it as either XML or a path tree
  If XML generator is used, the model is emitted as XML

  Shortcut to useModelXmlDocument().useRoot()

  NOTE: If the output buffer is not empty, it may be used instead
        since toutput generators often write output based on a model
        to the buffer
  */
  AXmlElement& useModel();

  /*!
  The output XML document (owner of the root xml element used as the model)
  A document holds header instructions and root xml element
  */
  AXmlDocument& useModelXmlDocument();

  /*!
  Get session data
  */
  AOSSessionData& useSessionData();

  /*!
  Persist session (if database persistence is enabled)
  */
  void persistSession();


  /*!
  Output buffer, sent to output after output stage
  If this is empty then XML document is the result
  
  NOTE: Putting data here will override the XML document, so you have to sent MIME type as needed
        Output generators will write output based on the model into this buffer and the server writes this to user
        It is recommended that this is used by output generators, but not required, you're the programmer
  */
  ARope& useOutputBuffer();

  /*!
  Set execution state of the context
  The old state is pushed into the queue with time in event
  @param execution state
  @param if this state is an error
  @param maximum time it should stay in this state (used in some cases to detect timeout)
  */
  void setExecutionState(const AEmittable&, bool isError = false, double maxTimeInState = INVALID_TIME_INTERVAL);
  
  /*!
  Set execution state of the context to the what() of the exception, this is always an error state
  The old state is pushed into the queue with time in event
  @param execution state exception treated as an error
  */
  void setExecutionState(const AException&);

  /*!
  Reset the execution state
  Pushes current state into the queue and stops the timer of the state
  */
  void resetExecutionState();

  /*!
  True if the current state max time is exceeded (timeout set by setExecutionState)
  @return true if the current state is over the max time specified
  */
  bool isStateTimedOut() const;

  /*!
  Access to the socket associated with this context
  This is provided for flexibility but XML document should be the preferred output
  Output buffer should be used when output is not XML
  If the output is written to socket manually please set the appropriate ContentFlag types (such as header sent, output sent, etc)
  Please use this with caution, it's here to make your life easy in some odd edge case, but be careful
  */
  AFile_Socket& useSocket();
    
  /*!
  Request HTTP header
  
  @return AHTTPRequestHeader object of the request
  */
  AHTTPRequestHeader& useRequestHeader();

  /*!
  Request cookies

  @see useRequestHeader().useCookies()
  @return ACookies of the request
  */
  ACookies& useRequestCookies();
  
  /*!
  Request URL

  @see useRequestHeader().useUrl()
  @return AUrl of the request
  */
  AUrl& useRequestUrl();
  
  /*!
  HTTP request parameters
  Initially this is only the query string
  Input processors add name/value pairs to this, so form and multi-part will be in here

  @see useRequestHeader().useUrl().useParameterPairs()
  @return AQueryString of the request
  */
  AQueryString& useRequestParameterPairs();

  /*!
  Response HTTP header

  @return AHTTPResponseHeader object used for output
  */
  AHTTPResponseHeader& useResponseHeader();
  
  /*!
  Response cookies

  @see useResponseHeader().useCookies()
  @return ACookies of the response header
  */
  ACookies& useResponseCookies();
  
  /*!
  Uses request header to determine which MIME type the response header will use
  */
  void setResponseMimeTypeFromRequestExtension();

  /*!
  Dumps self into output model depending on level
  
  @return
  0 - only dumps errors to dumpContext/context/error
  1 - adds context internals (executor will force XML display for >0)
  2 - adds 
    extended debugDump to dumpContext/context/debugDump
    configuration to dumpContext/configuration
  */
  void dumpContext(int dumpContextLevel);

  /*!
  AXmlEmittable

  @param element to receive the XML dump of this object
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  Checks if dumpContext is specified and is allowed

  returns: 0 - not specified or not allowed
           >0 - level value requested
  */
  int getDumpContextLevel() const;
  
  /*!
  Context specific data used to contain data for the duration of the request
  Also usable as inter-module communication
  */
  AObjectContainer& useContextObjects();

  /*!
  Gets the session object
  */
  void setSessionObject(AOSSessionData *);
  
  /*!
  Error message handling
  Calling this will include context dump in log
  addDebugDump will force a debugDump on this context and add it to the log (can be long and mainly for critical errors)
  */
  void addError(const AString& where, const AString& what, bool addDebugDump = false);
  void addError(const AString& where, const AException& what, bool addDebugDump = false);
  AEventVisitor& useEventVisitor();
  
  /*!
  Set response header to do a 302 redirect
  Does NOT set the context flag CTX_IS_REDIRECTING (that is set when AOSContext::RETURN_REDIRECT is detected by executor)
  */
  void setResponseRedirect(const AString& url);

  /*!
  Directrory config object (NULL if does not exist)
  */
  const AOSDirectoryConfig *getDirConfig() const;

  /*!
  Sets new command from path/commandname
  */
  bool setNewCommandPath(const AString& commandpath);

  /*!
  Lookup the command based on current request URL
  @return true if command is found
  */
  bool setCommandFromRequestUrl();

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
  Output committed
  */
  bool isOutputCommitted() const;
  void setOutputCommitted(bool b);

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
    CTXFLAG_IS_REDIRECTING,           //a_Context is in redirect mode
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

  /*!
  Object name used when inserting into ABasePtrHolder in scripting
  */
  static const AString OBJECTNAME;

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
