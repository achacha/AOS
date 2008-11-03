/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AContext_HPP__
#define INCLUDED__AContext_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AFile_Socket.hpp"
#include "AStringHashMap.hpp"
#include "ABasePtrContainer.hpp"
#include "ARope.hpp"
#include "AEventVisitor.hpp"
#include "ATimer.hpp"
#include "AXmlEmittable.hpp"
#include "AOSSessionData.hpp"
#include "AXmlDocument.hpp"
#include "ABitArray.hpp"
#include "ARandomAccessBuffer.hpp"
#include "ALuaTemplateContext.hpp"
#include "AOSController.hpp"

class AUrl;
class AQueryString;
class ACookies;
class AOSServices;
class AThread;
class AOSDirectoryConfig;

class AOS_BASE_API AOSContext : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Name of the root element
  */
  static const AString XML_ROOT;

  /*!
  Constants used inside the context model or reference to it
  */
  static const AString S_REQUEST;
  static const AString S_RESPONSE;
  static const AString S_SESSION;
  static const AString S_OUTPUT;
  static const AString S_GLOBAL;
  static const AString S_ERROR;
  static const AString S_MESSAGE;
  static const AString S_CONTEXT;
  static const AString S_MODEL;
  static const AString S_REFERER;
  static const AString S_GZIP;

public:
  /*!
  ctor

  @param pSocket pointer to the incomming socket connection, OWNED and DELETED
  @param services reference
  */
  AOSContext(AFile_Socket *pSocket, AOSServices& services);

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
  Finalize context, used before entering history
  Stopping timers and closing/deleting connection file

  If this is being called before caching in freestore for reuse, releaseMemory should be true to minimize carrying around stuff that is useless

  @param releaseMemory if true will try to release any memory it may have allocated (event visitor entries, etc)
  */
  void finalize(bool releaseMemory = false);

  /*!
  Clear out the context, delete the file
  Calls reset() then clean out client specific data (used after terminate)
  */
  void clear();

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

  @return bytes written
  */
  size_t writeResponseHeader();

  /*!
  Writes output buffer
  If it is empty it emits the output XML document into in and sets content type to text/xml
  If response header was not yet written, it will write it
  forceXmlDocument - uses XML document even if output buffer has data
  */
  void writeOutputBuffer(bool forceXmlDocument = false);

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
  Checks if the session data exists for this context
  Sessions are lazy-create, so unless you get the session object it is not created

  @return true if it already exists
  */
  bool existsSessionData() const;
  
  /*!
  Gets a value associate with a name in session data
  If session does not exist false will be returned and new session data will NOT be created

  @param path of the content to get
  @param value to emit content into
  @return true if session exists and name/value pair exists 
  */
  bool getSessionDataContent(const AString& path, AString& value);

  /*!
  If exists: looks uup the reference from the Cookie in th request
  If does not exist: new one will be created and added to session store and cookie will be associated with response

  NOTE: If you are looking to get a value from session variable or check for existance without forcing a create
        use getSessionDataValue or existsSessionData

  @return Reference to the session data object
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
  AOutputBuffer& useOutputBuffer();

  /*!
  Checks if the output buffer is empty
  */
  bool isOutputBufferEmpty() const;

  /*!
  Clears output buffer
  */
  void clearOutputBuffer();

  /*!
  Gets the size of the output buffer
  */
  size_t getOutputBufferSize() const;

  /*!
  Process a static page requested

  @return true if processed and handled, false if static page was loaded into output buffer
  */
  bool processStaticPage();

  /*!
  True if the current state max time is exceeded (timeout set by setExecutionState)
  @return true if the current state is over the max time specified
  */
  bool isStateTimedOut() const;

  /*!
  Is the connection forced to Close via request or response header
  @return true if we are to close the connection after processing
  */
  bool isConnectionClose() const;

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
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Checks if dumpContext is specified and is allowed

  returns: 0 - not specified or not allowed
           >0 - level value requested
  */
  int getDumpContextLevel() const;
  
  /*!
  Context specific data used to contain data for the duration of the request
  Also usable as inter-module communication

  Contains AString -> ABase * conatiner
  Objects will be DELETED after request is complete
  */
  ABasePtrContainer& useContextObjects();

  /*!
  Gets the session object, returns current

  @param pSessionData new session object
  @return old session object
  */
  AOSSessionData *setSessionObject(AOSSessionData *pSessionData);
    
  /*!
  Access the event visitor
  This is where all events for the context get added
  */
  AEventVisitor& useEventVisitor();
  
  /*!
  Error message handling
  Adds error to AEventVisitor of this context
  Adds error to the running log file and flushes it

  Calling this will include context dump in log
  @param where Location of the error
  @param what Error type
  @param addDebugDump will force a debugDump on this context and add it to the log (can be long and mainly for critical errors)
  */
  void addError(const AString& where, const AEmittable& what, bool addDebugDump = false);

  /*!
  Sets the locale on the request header  Accept-Language:
  This is used to lookup locale specific directories

  @param locale to set on the request
  */
  void setLocaleOnRequestHeader(const AString& locale);

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
  Sets new controller from path/controllername
  */
  bool setNewControllerPath(const AString& path);

  /*!
  Lookup the command based on current request URL
  @return true if command is found
  */
  bool setControllerFromRequestUrl();

  /*!
  Access the context specific Lua template context
  */
  ALuaTemplateContext& useLuaTemplateContext();

  /*!
  Controller object (NULL if does not exist)
  */
  const AOSController *getController() const;

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
    CONFLAG_ISAVAILABLE_SELECT_SET = 0, //a_IsAvailable queue did a select and socket was set
    CONFLAG_ISAVAILABLE_PENDING,        //a_IsAvailable queue did a select and either data is pending
    CONFLAG_IS_HTTP11_PIPELINING,       //a_HTTP 1.1 pipelining is being used
    CONFLAG_IS_SOCKET_ERROR,            //a_Socket in error state
    CONFLAG_IS_SOCKET_CLOSED,           //a_Remote socket was closed
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
    CTXFLAG_IS_CACHE_CONTROL_NO_CACHE,//a_Response will include no-cache for dynamic unless this is set
    CTXFLAG_IS_USING_SESSION_DATA,    //a_Set when session initialized the data on the context
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
  Object name used when inserting into ABasePtrContainer in scripting
  */
  static const AString OBJECTNAME;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:  
  //a_Read and parse Http header
  AOSContext::Status _processHttpHeader();

  //a_Wait until at least 1 byte is available on the channel
  bool _waitForFirstChar();

  //a_Event visitor
  AEventVisitor m_EventVisitor;
  
  //a_HTTP data
  AHTTPRequestHeader m_RequestHeader;
  AHTTPResponseHeader m_ResponseHeader;

  //a_Output buffer
  ARope m_OutputBuffer;

  //a_Context objects
  ABasePtrContainer m_ContextObjects;

  //a_Output document
  AXmlDocument m_OutputXmlDocument;

  //a_Configuration
  AOSServices& m_Services;

  //a_Associated session if any
  AOSSessionData *mp_SessionObject;

  /*!
  Controller of this context (cached to prevent constant lookup in AOSConfiguration)
  */
  const AOSController *mp_Controller;
  
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

  /*!
  Lua template context
  */
  ALuaTemplateContext *mp_LuaTemplateContext;
  u4 m_DefaultLuaLibraries;

  //a_No copying
  AOSContext& operator =(const AOSContext&) { return *this; }

  //a_Socket associated with context
  AFile_Socket *mp_RequestFile;

  //a_Post process http request object after it is read and parsed
  AOSContext::Status _postProcessHttpHeader();

  //a_Determines if the current output should be gzip'd, 0 if not to compress, else 1-9 compression level
  int _calculateGZipLevel(size_t documentSize);

  //a_Peek to socket
  size_t _write(ARandomAccessBuffer&, size_t);
};

#endif // INCLUDED__AContext_HPP__
