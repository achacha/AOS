/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AHTTPRequestHeader_HPP__
#define INCLUDED__AHTTPRequestHeader_HPP__

#include "apiABase.hpp"
#include "AHTTPHeader.hpp"
#include "AUrl.hpp"
#include "ACookies.hpp"

class ABASE_API AHTTPRequestHeader : public AHTTPHeader
{
public:
  /*!
  HTTP request method string constants
  */
  static const AString METHOD_CONNECT;
  static const AString METHOD_GET;
  static const AString METHOD_HEAD; 
  static const AString METHOD_OPTIONS;
  static const AString METHOD_POST;
  static const AString METHOD_PUT;
  static const AString METHOD_DELETE; 
  static const AString METHOD_TRACE;

  /*!
  Method enumeration used with getMethodId
  */
  enum METHOD_ID
  {
    METHOD_ID_CONNECT,
    METHOD_ID_GET,
    METHOD_ID_HEAD, 
    METHOD_ID_OPTIONS,
    METHOD_ID_POST,
    METHOD_ID_PUT,
    METHOD_ID_DELETE, 
    METHOD_ID_TRACE,
    METHOD_ID_UNKNOWN = -1
  };

public:
  AHTTPRequestHeader();
  AHTTPRequestHeader(const AString &strHeader);
  AHTTPRequestHeader(const AHTTPRequestHeader&);
  AHTTPRequestHeader& operator=(const AHTTPRequestHeader&);
  virtual ~AHTTPRequestHeader();

  //a_Presentation
  virtual void emit(AOutputBuffer& target) const;
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  //a_Clear header
  virtual void clear();

  /**
   * Displays the request that is proxy compatible
   *   i.e. contains the full URL in the first line of the requets
   *   including the http://..server../ etc...
   *   usually witout proxy mode the server is omited
   *   since you are already connected to it
  */
  void emitProxyHeader(AOutputBuffer&) const;

  /*!
  Parse URL and set Host:
  */
  void parseUrl(const AString& str);
  
  /*!
  Method string
  Whatever was in the HTTP header
  */
  const AString& getMethod() const;

  /*!
  Method id
  Maps into enum for all known types and METHOD_ID_UNKNOWN otherwise
  */
  AHTTPRequestHeader::METHOD_ID getMethodId() const;

  //a_Object access
  const AUrl& getUrl() const;
  const ACookies& getCookies() const;
  AUrl& useUrl();
  ACookies& useCookies();

  //a_Setting methods
  void setMethod(const AString& strNewMethod);
  void parseCookie(const AString& strCookieLine);

  /*!
  Helpers
  */
  bool isHttpPipeliningEnabled() const;

  /*!
  Is method valid
  */
  bool isValidMethod() const;

  /*!
  Is path valid
  */
  bool isValidPath() const;

  /*!
  Is path valid
  */
  bool isValidHttp() const;

  /*!
  If-Modified-Since converted to ATime
  */
  ATime getIfModifiedSince() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //a_First line for request
  //a_[METHOD] [SP] [URI] [SP] [HTTPVERSION]
  virtual bool _parseLineZero();
  
  //a_Pairs that the child will handle
  virtual bool _handledByChild(const ANameValuePair &nvHTTPPair);

  void _copy(const AHTTPRequestHeader&);

  AHTTPRequestHeader::METHOD_ID _lookupMethodId(const AString&) const;

private:
  AString mstr_Method;
  AHTTPRequestHeader::METHOD_ID m_MethodId;
  AUrl murl_Request;
  ACookies mcookies_Request;
};

#endif // INCLUDED__IWHTTPRequestHeader_HPP__
