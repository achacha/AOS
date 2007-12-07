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
  //a_Ctors and dtor
  ACookie();
  ACookie(const ACookie& cookieSource);
  ACookie(const AString &strName, const AString &strValue = "");
  ~ACookie();              //a_Make virtual if adding virtual methods

  //a_Assignment operator (overlay implied)
  ACookie& operator =(const ACookie &cookieSource);

  //a_Class functions
  void clear();       //a_Clears the cookie

  //a_NameValue operations
  void setNameValue(const AString &strName, const AString &strValue);
  void setValue(const AString &strValue) { m_strValue = strValue; }

  //a_Special set expired flag
  void setExpired(bool boolFlag = TRUE) { m_boolExpired = boolFlag; }
  bool isExpired()                      { return m_boolExpired; }
  void setNoExpire()                    { m_boolExpired= FALSE; m_boolExpirationSet = FALSE; }

  //a_Setting methods
  void setDomain(const AString &strDomain)   { m_strDomain   = strDomain;  }
  void setPath(const AString &strPath)       { m_strPath     = strPath;    }
  void setSecure(bool boolSecure = true)     { m_boolSecure  = boolSecure; }

  /*!
  Cookie version
  0 = old style netscape cookie without a version attribute
  */
  void setVersion(int iVersion)             { m_iVersion    = iVersion;   }
  
  /*!
  Either can be used to set expiry
  */
  void setExpires(const ATime &timeExpires);
  void setMaxAge(long lMaxAge);

  //a_Setting these will force VERSION=1, as per RFC-2109
  void setComment(const AString &strComment) { m_strComment = strComment; m_iVersion = 1; }

  //a_Access methods
  const AString &getName()  const   { return m_strName; }
  const AString &getValue() const   { return m_strValue; }
  const ATime   &getExpires() const { return m_timeExpires; }
  const AString &getDomain() const  { return m_strDomain; }
  const AString &getPath() const    { return m_strPath; }
  bool  isSecure() const            { return m_boolSecure; }

  //a_RFC-2109 specific
  int  getVersion() const           { return m_iVersion; }
  long getMaxAge() const            { return m_lMaxAge; }
  const AString &getComment() const { return m_strComment; }

  //a_Response is sent by the server and contains all fields (one per SET-COOKIE: lines)
  void emitResponseHeaderString(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer& target) const;            //a_Default is a server response cookie
  virtual void emitXml(AXmlElement&) const;

  //a_Request is sent by the browser and only contains name=value pairs (many on COOKIE: line)
  void emitRequestHeaderString(AOutputBuffer&) const;

  //a_Comparisson returns 0 if equal
  int compare(ACookie&) const;

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

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif

