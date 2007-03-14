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
  void setValue(const AString &strValue) { m__strValue = strValue; }

  //a_Special set expired flag
  void setExpired(bool boolFlag = TRUE) { m__boolExpired = boolFlag; }
  bool isExpired()                      { return m__boolExpired; }
  void setNoExpire()                    { m__boolExpired= FALSE; m__boolExpirationSet = FALSE; }

  //a_Setting methods
  void setDomain(const AString &strDomain)   { m__strDomain   = strDomain;  }
  void setPath(const AString &strPath)       { m__strPath     = strPath;    }
  void setSecure(bool boolSecure = true)     { m__boolSecure  = boolSecure; }

  //a_RFC-2109 specific
  void setVersion(int iVersion)             { m__iVersion    = iVersion;   }
  void setExpires(const ATime &timeExpires);
  void setMaxAge(long lMaxAge);

  //a_Setting these will force VERSION=1, as per RFC-2109
  void setComment(const AString &strComment) { m__strComment = strComment; m__iVersion = 1; }

  //a_Access methods
  const AString &getName()  const   { return m__strName; }
  const AString &getValue() const   { return m__strValue; }
  const ATime   &getExpires() const { return m__timeExpires; }
  const AString &getDomain() const  { return m__strDomain; }
  const AString &getPath() const    { return m__strPath; }
  bool  isSecure() const            { return m__boolSecure; }

  //a_RFC-2109 specific
  int  getVersion() const           { return m__iVersion; }
  long getMaxAge() const            { return m__lMaxAge; }
  const AString &getComment() const { return m__strComment; }

  //a_Response is sent by the server and contains all fields (one per SET-COOKIE: lines)
//  AString toResponseHeaderString() const;
  void emitResponseHeaderString(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer& target) const;            //a_Default is a server response cookie
  virtual void emit(AXmlElement&) const;

  //a_Request is sent by the browser and only contains name=value pairs (many on COOKIE: line)
//  AString toRequestHeaderString() const;
  void emitRequestHeaderString(AOutputBuffer&) const;

  //a_Comparisson returns 0 if equal
  int compare(ACookie&) const;

private:
  //a_Internal methods
  void __copy(const ACookie &cookieSource);
  
  //a_COOKIE specific
  AString  m__strName;
  AString  m__strValue;
  AString  m__strPath;
  AString  m__strDomain;
  ATime    m__timeExpires;       //a_Depending on version, will use wither expires or Max-Age
  bool     m__boolSecure;
  
  //a_Class specific
  bool m__boolExpirationSet; //a_Will be true if the user set the expiration, else this will cause EXPIRES= to be omitied and cookie to exist indefinitely
  bool m__boolExpired;       //a_When set to true, when cookie is generated it will be expired depending on which version of the standard is used

  //a_RFC-2109 specific
  int     m__iVersion;
  AString m__strComment;
  long    m__lMaxAge;           //a_(not yet supported 1/98, use expire functions)

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif

