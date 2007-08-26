#ifndef INCLUDED__AHTTPRequestHeader_HPP__
#define INCLUDED__AHTTPRequestHeader_HPP__

#include "apiABase.hpp"
#include "AHTTPHeader.hpp"
#include "AUrl.hpp"
#include "ACookies.hpp"

class ABASE_API AHTTPRequestHeader : public AHTTPHeader
{
public:
  AHTTPRequestHeader();
  AHTTPRequestHeader(const AString &strHeader);
  AHTTPRequestHeader(const AHTTPRequestHeader&);
  AHTTPRequestHeader& operator=(const AHTTPRequestHeader&);
  virtual ~AHTTPRequestHeader();

  //a_Presentation
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AXmlElement&) const;

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

  //a_Access methods
  inline const AString& getMethod() const;

  //a_Object access
  inline const AUrl& getUrl() const;
  inline const ACookies& getCookies() const;
  inline AUrl& useUrl();
  inline ACookies& useCookies();

  //a_Setting methods
  void setMethod(const AString& strNewMethod);
  void parseCookie(const AString& strCookieLine);

  /*!
  Is a POST
  */
  bool isPOST() const;

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

protected:
  //a_First line for request
  //a_[METHOD] [SP] [URI] [SP] [HTTPVERSION]
  virtual bool _parseLineZero();
  
  //a_Pairs that the child will handle
  virtual bool _handledByChild(const ANameValuePair &nvHTTPPair);

  void _copy(const AHTTPRequestHeader&);

private:
  AString  mstr_Method;
  AUrl     murl_Request;
  ACookies mcookies_Request;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__IWHTTPRequestHeader_HPP__
