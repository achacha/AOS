#ifndef INCLUDED__ACookie_HPP__
#define INCLUDED__ACookie_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ATime.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ABASE_API ACookie : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  ctors
  Default version is 1.0
  */
  ACookie();
  ACookie(const ACookie& cookieSource);
  ACookie(const AString &strName, const AString &strValue = "");
  
  /*!
  dtor
  */
  virtual ~ACookie();

  //a_Assignment operator (overlay implied)
  ACookie& operator =(const ACookie &cookieSource);

  //a_Class functions
  void clear();       //a_Clears the cookie

  //a_NameValue operations
  void setNameValue(const AString &strName, const AString &strValue);
  void setValue(const AString &strValue);

  //a_Special set expired flag
  void setExpired(bool boolFlag = true);
  bool isExpired();
  void setNoExpire();

  //a_Setting methods
  void setDomain(const AString &strDomain);
  void setPath(const AString &strPath);
  void setSecure(bool boolSecure = true);

  /*!
  Cookie version
  0 = old style netscape cookie without a version attribute
  1 = new style (RFC-2109) NOTE: IE does not support this correctly as of IE 7
  */
  void setVersion(int iVersion);
  
  /*!
  Either can be used to set expiry, version determines which is emitted
  version 0 will use EXPIRES=
  version 1 will use MAX-AGE=
  */
  void setExpires(const ATime &timeExpires);
  void setMaxAge(long lMaxAge);

  //a_Setting these will force VERSION=1, as per RFC-2109
  void setComment(const AString &strComment);

  //a_Access methods
  const AString& getName() const;
  const AString& getValue() const;
  const ATime& getExpires() const;
  const AString& getDomain() const;
  const AString& getPath() const;
  bool isSecure() const;

  //a_RFC-2109 specific
  int getVersion() const;
  long getMaxAge() const;
  const AString& getComment() const;

  //a_Response is sent by the server and contains all fields (one per SET-COOKIE: lines)
  void emitResponseHeaderString(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer& target) const;            //a_Default is a server response cookie
  virtual void emitXml(AXmlElement&) const;

  //a_Request is sent by the browser and only contains name=value pairs (many on COOKIE: line)
  void emitRequestHeaderString(AOutputBuffer&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Internal methods
  void __copy(const ACookie &cookieSource);
  
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

