#ifndef INCLUDED__AOSOutputContext_HPP__
#define INCLUDED__AOSOutputContext_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AOSContext.hpp"

class AOSConfiguration;

/*!
 High level wrapper to AOSContext that is configured for output only
*/
class AOS_BASE_API AOSOutputContext : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Constructed based on an existing AOSContext
  useLocalOutput if true will create a local object for output instead of the useOutput() from AContext (mostly for debugging)
  **/
  AOSOutputContext(AOSContext& context);
  virtual ~AOSOutputContext();

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;
  
  /*!
  Response header and sub-object helpers (read/write)
  */
  AHTTPResponseHeader& useResponseHeader();
  ACookies& useResponseCookies();
  void setResponseMimeTypeFromRequestExtension();     //a_Uses request header to determine which MIME type the response header will use

  /*!
  Request header and sub-object helpers (read-only)
  */
  const AHTTPRequestHeader& getRequestHeader() const;
  const ACookies& getRequestCookies() const;
  const AUrl& getRequestUrl() const;
  const AQueryString& getRequestParameterPairs() const;

  /*!
  Read-Only objects
  */
  AString                   getOutputCommand() const;
  const AObjectContainer&   getContextObjects() const;
  const AStringHashMap&     getSessionData() const;
  const AXmlElement&        getOutputParams() const;
  const AXmlDocument&       getOutputXmlDocument() const;
  const AXmlElement&        getOutputRootXmlElement() const;
  const AOSConfiguration&   getConfiguration() const;

  /*!
  Access to the socket associated with this context (useOutput() is the best way to output)
  */
  AFile_Socket& useSocket();

  /*!
  Event visitor
  */
  AEventVisitor& useEventVisitor();

  /*!
  Output committed
  */
  bool isOutputCommitted() const;
  void setOutputCommitted(bool b);

  /*!
  Output buffer, if this is used, then XML document output is supressed (not recommended unless you are not sending XML)
  The XML document is the place where modules will place results, sending data here
    will override it, you should emit the XML document here if you don't want to throw it away
  */
  ARope& useOutputBuffer();

  /*!
    Error logging
  */
  void addError(const AString& where, const AString& what);

protected:
  //a_Context
  AOSContext& m_Context;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif  // INCLUDED__AOSOutputContext_HPP__

