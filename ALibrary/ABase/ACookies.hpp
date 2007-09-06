#ifndef INCLUDED__ACookies_HPP__
#define INCLUDED__ACookies_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ACookie.hpp"
#include "AXmlEmittable.hpp"

class ABASE_API ACookies: public ADebugDumpable, public AXmlEmittable
{
public:
  //a_Ctors, and dtor
  ACookies();
  ACookies(const AString &strLine);           //a_This is the HTTP_COOKIE line that the browser sends
  ACookies(const ACookies &cookiesSource);
  ACookies &operator =(const ACookies& cookiesSource);
  ~ACookies();                               //a_Make virtual if adding virtual functions

  virtual void clear();
  virtual void parse(const AString &strLine);
  
  //a_Response header's output (Cookie: NAME=VALUE; parameters ...) multiple lines
  virtual void emitResponseHeaderString(AOutputBuffer&) const;

  //a_Request header's output (NAME=VALUE; NAME=VALUE; ...) one line
  void emitRequestHeaderString(AOutputBuffer&) const;

  //a_Default output is request header one line
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AXmlElement&) const;

  //a_Specialized method used to parse 'Set-Cookie' token in the response header
  //a_This line contains cookie name=value and parameters (which are for that cookie)
  void parseSetCookieLine(const AString &);

  //a_Access to cookies
  u4 size() const { return (u4)mvector__Cookies.size(); }
  bool exists(const AString& name) const;
  bool getValue(const AString& name, AString&) const;
  bool getValue(const AString& name, AString&, const AString &strDefault) const;
  
  //a_Returns true if found and copies the cookies
  //a_If not found only false is returned and cookie is not touched
  bool getCookie(const AString& name, ACookie& cookieTarget) const;

  //a_Adds a cookie of given name and returns the reference of the new cookie object
  ACookie &addCookie(const AString& name, const AString &strValue = AConstant::ASTRING_EMPTY);

  void expireCookie(const AString& name);     //a_Expires/Removes it on the client-side
  void removeCookie(const AString& name);     //a_Deletes locally

private:
  //a_Internal cookie storage
  typedef std::vector<ACookie *> VECTOR_CookiePointers;
  VECTOR_CookiePointers mvector__Cookies;

  //a_Object maintenance
  void __copy(const ACookies &cookiesSource);
  
  //a_Cookie access
  ACookie *__findCookie(const AString& name) const;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif


