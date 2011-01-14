/*
Written by Alex Chachanashvili

$Id: AHTTPRequestHeader.hpp 310 2009-10-16 21:40:38Z achacha $
*/
#ifndef INCLUDED__AHTTPRequestHeader_HPP__
#define INCLUDED__AHTTPRequestHeader_HPP__

#include "apiAPlatform.hpp"
#include "AHTTPHeader.hpp"
#include "AUrl.hpp"
#include "ACookies.hpp"

/*!
HTTP request header class
*/
class APLATFORM_API AHTTPRequestHeader : public AHTTPHeader
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
  //! default ctor
  AHTTPRequestHeader();
  
  //! ctor with parse
  AHTTPRequestHeader(const AString &strHeader);
  
  //! copy ctor
  AHTTPRequestHeader(const AHTTPRequestHeader&);
  
  //! assign operator
  AHTTPRequestHeader& operator=(const AHTTPRequestHeader&);
  
  //! virtual dtor
  virtual ~AHTTPRequestHeader();

  /*!
  AEmittable

  @param target to emit to
  */
  virtual void emit(AOutputBuffer& target) const;
  
  /*!
  AXmlEmittable

  @param thisRoot to emit into
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Clear header
  */
  virtual void clear();

  /*!
  Displays the request that is proxy compatible
    i.e. contains the full URL in the first line of the requets including the http://..server../ etc...
         usually witout proxy mode the server is omited since you are already connected to it

  @param target to emit proxy header to
  */
  void emitProxyHeader(AOutputBuffer& target) const;

  /*!
  Parse URL and set Host:

  @param url to parse and add to the header, server will be set to Host: field
  */
  void parseUrl(const AString& url);
  
  /*!
  Method string
  Whatever was in the HTTP header
  
  @return constant reference to method AString
  */
  const AString& getMethod() const;

  /*!
  Method id
  Maps into enum for all known types and METHOD_ID_UNKNOWN otherwise
  
  @return method id
  */
  AHTTPRequestHeader::METHOD_ID getMethodId() const;

  /*!
  Access the url object

  @return constant reference to AUrl
  */
  const AUrl& getUrl() const;

  /*!
  Access the cookie container

  @return constant reference to the ACookies container
  */
  const ACookies& getCookies() const;
  
  /*!
  Access the url object

  @return reference to AUrl
  */
  AUrl& useUrl();
  
  /*!
  Access the cookie container

  @return reference to the ACookies container
  */
  ACookies& useCookies();

  /*!
  Set the method

  @param strNewMethod new method name (all caps and must be valid), use AHTTPRequestHeader::METHOD_* AString constants
  */
  void setMethod(const AString& strNewMethod);
  
  /*!
  Parse (additional) the cookie line
  fromAFile will read and parse the entire header but this can be used to parse additional header lines

  @param strCookieLine to parse and add to ACookies container from "Cookie: name=value; ..."
  */
  void parseCookie(const AString& strCookieLine);

  /*!
  Does the request ask for http pipelining to be enabled

  @return true if HTTP pipelining is requested
  */
  bool isHttpPipeliningEnabled() const;

  /*!
  Is method valid

  @return true if method is valid
  */
  bool isValidMethod() const;

  /*!
  Is path valid
  
  @see AUrl::isValid
  @return AUrl.isValid for the request path
  */
  bool isValidPath() const;

  /*!
  Is HTTP valid

  HTTP/1.1 must have Host: pair
  
  @return true if HTTP validity test is successful
  */
  bool isValidHttp() const;

  /*!
  Checks if the request is a FORM POST
  
  @return true if a POST
  */
  bool isFormPost() const;

  /*!
  Checks if the request is a FORM multi-part POST
  
  @return true if POST and multi-part
  */
  bool isFormMultiPartPost() const;

  /*!
  If-Modified-Since converted to ATime

  @return ATime object that is used to compare if content has changed since
  */
  ATime getIfModifiedSince() const;

  /*!
  Gets an ordered list of accepted languages based on q (quality) value in Accept-Language: header pair

  Language is xx-YY  where xx is language code and YY is the localized version of that language
  e.g.
    en      - All english languages
    en-US   - US specific english
    en-GB   - GB specific english

  @param target that will contain languages, will clear the list before adding languages in sorted order
  @return number of languages found in Accept-Language: header pair
  */
  size_t getAcceptLanguageList(LIST_AString& target) const;

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
