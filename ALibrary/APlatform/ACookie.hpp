/*
Written by Alex Chachanashvili

$Id: ACookie.hpp 247 2008-09-26 20:19:37Z achacha $
*/
#ifndef INCLUDED__ACookie_HPP__
#define INCLUDED__ACookie_HPP__

#include "apiAPlatform.hpp"
#include "AString.hpp"
#include "ATime.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class APLATFORM_API ACookie : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  ctors
  Default version is 1.0
  */
  ACookie();
  
  /*!
  Copy ctor
  */
  ACookie(const ACookie& cookieSource);
  
  /*!
  ctor

  @param name of the cookie
  @param value of the cookie (can be empty)
  @param domain of the cookie (can be empty)
  @param path of the cookie (defaults to empty which implies root)
  */
  ACookie(
    const AString &name, 
    const AString &value = AConstant::ASTRING_EMPTY,
    const AString &domain = AConstant::ASTRING_EMPTY,
    const AString &path = AConstant::ASTRING_EMPTY
  );
  
  /*!
  dtor
  */
  virtual ~ACookie();

  //! Assignment operator (overlay implied)
  ACookie& operator =(const ACookie &cookieSource);

  //! Clear the cookie
  void clear();

  /*!
  Set name and value

  @param name of the cookie pair
  @param value of the cookie pair
  */
  void setNameValue(const AString &name, const AString &value);
  
  /*!
  Set value

  @param value of cookie pair
  */
  void setValue(const AString &value);

  /*!
  Special set expired flag

  @param boolFlag if true the cookie will be expired
  */
  void setExpired(bool boolFlag = true);
  
  /*!
  Check if inspired

  @return true if expired
  */
  bool isExpired();
  
  /*!
  Remove expiration if any
  */
  void setNoExpire();

  /*!
  Set domain
  
  @param domain of cookie
  */
  void setDomain(const AString& domain);

  /*!
  Set path
  
  @param path of cookie
  */
  void setPath(const AString& path);

  /*!
  Set secure cookie
  
  @param secure true/false
  */
  void setSecure(bool secure = true);

  /*!
  Cookie version
  0 = old style netscape cookie without a version attribute
  1 = new style (RFC-2109) NOTE: IE does not support this correctly as of IE 7
  
  @param version of the cookie
  */
  void setVersion(int version);
  
  /*!
  Either can be used to set expiry, version determines which is emitted
  version 0 will use EXPIRES=
  version 1 will use MAX-AGE=
  
  @param timeExpires when the cookie should expire
  */
  void setExpires(const ATime& timeExpires);

  /*!
  Either can be used to set expiry, version determines which is emitted
  version 0 will use EXPIRES=
  version 1 will use MAX-AGE=
  
  @param lMaxAge number of seconds until cookie expires
  */
  void setMaxAge(long lMaxAge);

  /*!
  Setting these will force VERSION=1, as per RFC-2109
  
  @param strComment to set on cookie and force to version 1 (this is not available in version 0)
  */
  void setComment(const AString &strComment);

  /*!
  Get cookie name

  @return cookie name
  */
  const AString& getName() const;

  /*!
  Get cookie value

  @return cookie value
  */
  const AString& getValue() const;

  /*!
  Get expiration time

  @return constant reference to ATime object
  */
  const ATime& getExpires() const;

  /*!
  Get cookie domain name

  @return domain name
  */
  const AString& getDomain() const;
  
  /*!
  Get cookie path

  @return cookie path
  */
  const AString& getPath() const;
  
  /*!
  Is cookie secure

  @return true if secure cookie
  */
  bool isSecure() const;

  /*!
  Get cookie version
  RFC-2109 specific

  @return cookie version
  */
  int getVersion() const;

  /*!
  Get max age before cookie expires in seconds
  RFC-2109 specific

  Internally single ATime object is used for all expiration

  @return seconds to expiration
  */
  long getMaxAge() const;
  
  /*!
  Get comment
  RFC-2109 specific

  @return comment
  */
  const AString& getComment() const;

  /*!
  Response is sent by the server and contains all fields (one per SET-COOKIE: lines)

  @param target to append the server's response cookie line to
  */
  void emitResponseHeaderString(AOutputBuffer& target) const;
  
  /*!
  Request is sent by the browser and only contains name=value pairs (many on COOKIE: line)

  @param target to append the browser's request cookie line to
  */
  void emitRequestHeaderString(AOutputBuffer& target) const;

  /*!
  AEmittable
  Emit will output the server's response cookie

  @param target to append to
  @see emitResponseHeaderString
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AXmlEmittable

  @param thisRoot to append XML elements to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Internal methods
  void _copy(const ACookie &cookieSource);
  
  //a_COOKIE specific
  AString  m_strName;
  AString  m_strValue;
  AString  m_strPath;
  AString  m_strDomain;
  ATime    m_timeExpires;       //a_Depending on version, will use wither expires or Max-Age
  bool     m_boolSecure;
  
  //a_Class specific
  bool m_boolExpirationSet; //a_Will be true if the user set the expiration, else this will cause EXPIRES= to be omitied and cookie to exist indefinitely
  bool m_boolExpired;       //a_When set to true, when cookie is generated it will be expired depending on which version of the standard is used

  //a_RFC-2109 specific
  int     m_iVersion;
  AString m_strComment;
  long    m_lMaxAge;
};

#endif

