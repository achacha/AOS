/*
Written by Alex Chachanashvili

$Id: ACookies.hpp 247 2008-09-26 20:19:37Z achacha $
*/
#ifndef INCLUDED__ACookies_HPP__
#define INCLUDED__ACookies_HPP__

#include "apiAPlatform.hpp"
#include "AString.hpp"
#include "ACookie.hpp"
#include "AXmlEmittable.hpp"

class APLATFORM_API ACookies: public ADebugDumpable, public AXmlEmittable
{
public:
  //! Container used to store the cookie pointers
  typedef std::vector<ACookie *> CONTAINER;

public:
  //a_Ctors, and dtor
  ACookies();
  ACookies(const AString &strLine);           //a_This is the HTTP_COOKIE line that the browser sends
  ACookies(const ACookies &cookiesSource);
  ACookies &operator =(const ACookies& cookiesSource);
  ~ACookies();                               //a_Make virtual if adding virtual functions

  /*!
  Clear (remove all) the cookies
  */
  virtual void clear();
  
  /*!
  Parse a 'Cookie:' line
  Given a header with "Cookie: DATA", the DATA part is what gets parsed, the HTTP token is assumed to be removed
  */
  virtual void parse(const AString &);
  
  /*!
  Specialized method used to parse 'Set-Cookie:' token in the response header
  This line contains cookie name=value and parameters (which are for that cookie)
  */
  void parseSetCookieLine(const AString &);

  /*!
  Response header's output (Cookie: NAME=VALUE; parameters ...) multiple lines
  */
  virtual void emitResponseHeaderString(AOutputBuffer&) const;

  /*!
  Request header's output (NAME=VALUE; NAME=VALUE; ...) one line
  */
  void emitRequestHeaderString(AOutputBuffer&) const;

  /*!
  AEmittable
  Default output is request header one line
  */
  virtual void emit(AOutputBuffer&) const;
  
  /*!
  AXmlEmittable
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Number of cookie objects
  */
  size_t size() const;
  
  /*!
  Check if a cookie exists
  */
  bool exists(const AString& name) const;
  
  /*!
  Get value of the cookie

  @param name of the cookie
  @param value will be appended if found
  @return true if found
  */
  bool getValue(const AString& name, AString& value) const;
  
  /*!
  Get value of cookie or default if not found
  
  @param name of the cookie
  @param value will be appended if found or default
  @param strDefault value to use if cookie not found
  @return true if found
  */
  bool getValue(const AString& name, AString& value, const AString &strDefault) const;
  
  /*!
  Returns true if found and copies the cookies
  If not found only false is returned and cookie is not touched
  
  @param name of the cookie
  @param cookieTarget that will receive a copy of the cookie if it exists
  @return true if found
  */
  bool getCookie(const AString& name, ACookie& cookieTarget) const;

  /*!
  Adds a cookie of given name and returns the reference of the new cookie object

  @param name of the cookie (must not be empty)
  @param value of the cookie can be empty

  @return reference to the newly created cookie that is also added to this collection
  */
  ACookie &addCookie(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);

  /*!
  Expire a cookie by setting expiration date in the past

  @param name of the cookie (must not be empty)
  */
  void expireCookie(const AString& name);
  
  /*!
  Remove the cookie from the collection

  @param name of the cookie (must not be empty)
  */
  void removeCookie(const AString& name);     //a_Deletes locally

  /*!
  Get the cookie container
  */
  const ACookies::CONTAINER& getContainer() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Internal cookie storage
  CONTAINER m_Cookies;

  //a_Object maintenance
  void _copy(const ACookies &cookiesSource);
  
  //a_Cookie access
  ACookie *__findCookie(const AString& name) const;
};

#endif


