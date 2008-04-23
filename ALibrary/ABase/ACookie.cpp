#include "pchABase.hpp"
#include "ACookie.hpp"
#include "AException.hpp"
#include "ATextConverter.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlElement.hpp"

#define DEFAULT_VERSION 0

void ACookie::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ACookie @ " << std::hex << this << std::dec << ") { " << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_strName=" << m_strName << std::endl;   
  ADebugDumpable::indent(os, indent+1) << "m_strValue=" << m_strValue << std::endl;  
  ADebugDumpable::indent(os, indent+1) << "m_strPath=" << m_strPath << std::endl;   
  ADebugDumpable::indent(os, indent+1) << "m_strDomain=" << m_strDomain << std::endl; 
  ADebugDumpable::indent(os, indent+1) << "m_boolSecure=" << m_boolSecure << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_boolExpired=" << m_boolExpired << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_boolExpirationSet=" << m_boolExpirationSet << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_iVersion=" << m_iVersion << std::endl;  
  ADebugDumpable::indent(os, indent+1) << "m_lMaxAge=" << m_lMaxAge << std::endl;  
  ADebugDumpable::indent(os, indent+1) << "m_strComment=" << m_strComment << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_timeExpires={";
  m_timeExpires.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) <<"}" << std::endl;
  
  ADebugDumpable::indent(os, indent) <<"}" << std::endl;
}

ACookie::ACookie() :
  m_boolExpired(false),
  m_iVersion(DEFAULT_VERSION),
  m_boolSecure(false),
  m_boolExpirationSet(false), //a_Indefinite cookie
  m_lMaxAge(-1)
{
}

ACookie::ACookie(const AString &strName, const AString &strValue) :
  m_boolExpired(false),
  m_iVersion(DEFAULT_VERSION),
  m_boolSecure(false),
  m_strName(strName),
  m_strValue(strValue),
  m_boolExpirationSet(false), //a_Indefinite cookie
  m_lMaxAge(-1)
{
}

void ACookie::setNameValue(const AString &strName, const AString &strValue)
{
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  m_strName = strName;
  m_strValue = strValue;
}

ACookie::ACookie(const ACookie& cookieSource)
{
  __copy(cookieSource);
}

ACookie::~ACookie()
{
}

void ACookie::__copy(const ACookie &cookieSource)
{
  m_strName           = cookieSource.m_strName;   
  m_strValue          = cookieSource.m_strValue;  
  m_strPath           = cookieSource.m_strPath;   
  m_strDomain         = cookieSource.m_strDomain; 
  m_timeExpires       = cookieSource.m_timeExpires;
  m_boolSecure        = cookieSource.m_boolSecure;
                            
  m_boolExpired       = cookieSource.m_boolExpired;
  m_boolExpirationSet = cookieSource.m_boolExpirationSet;
 
  m_iVersion          = cookieSource.m_iVersion;  
  m_lMaxAge           = cookieSource.m_lMaxAge;
  m_strComment        = cookieSource.m_strComment;
}

//a_Used before overlaying the cookie (faster than delete/new)
void ACookie::clear()
{
  m_strName.clear();          
  m_strValue.clear();         
  m_strPath.clear();          
  m_strDomain.clear();        
  m_timeExpires.setToNow();
  m_boolSecure = false;      
                      
  m_boolExpired       = false;
  m_boolExpirationSet = false;
 
  m_iVersion = DEFAULT_VERSION;
  m_lMaxAge = -1;
  m_strComment.clear();       
}

//a_Assignment operator (overlay implied)
ACookie &ACookie::operator =(const ACookie & cookieSource)
{
  __copy(cookieSource);

  return *this;
}

void ACookie::emit(AOutputBuffer& target) const 
{ 
  emitResponseHeaderString(target);
}

AXmlElement& ACookie::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());
  
  thisRoot.addElement(ASW("name",4)).addData(m_strName);
  thisRoot.addElement(ASW("value",5)).addData(m_strValue);
  thisRoot.addElement(ASW("domain",6)).addData(m_strDomain);
  thisRoot.addElement(ASW("path",4)).addData(m_strPath);
  thisRoot.addElement(ASW("version",7)).addData(m_iVersion);
  thisRoot.addElement(ASW("comment",7)).addData(m_strComment);
  thisRoot.addElement(ASW("maxage",6)).addData(AString::fromS4(m_lMaxAge));
  
  AString str;
  m_timeExpires.emitRFCtime(str);
  thisRoot.addElement(ASW("expires",7), str);
  
  if (m_boolSecure) thisRoot.addElement(ASW("secure",6));

  return thisRoot;
}

void ACookie::emitResponseHeaderString(AOutputBuffer& target) const
{
  //a_First the NAME=VALUE must be set; "=" is actually optional, but netscape needs it (due to misimplementation)
  target.append(m_strName);
  target.append(AConstant::ASTRING_EQUALS);
  target.append(m_strValue);

  //a_Now let's figure out the expiration
  if (m_boolExpired)
  {
    //a_Cookie is to be expired on the client
    if (m_iVersion == 1)
      target.append("; Max-Age=0", 11);
    else
       target.append("; Expires=Tue, 01-Jan-1980 00:00:01 UT", 38);
  }
  else if (m_boolExpirationSet)
  {
    //a_Expiration was set at some point, use it, else no EXPIRES is needed
    if (m_iVersion == 1)
    {
      ATime t;    //a_Time now

      target.append("; Max-Age=", 10);
      if (-1 != m_lMaxAge)
        target.append(AString::fromS4(m_lMaxAge));
      else
        target.append(AString::fromS4((s4)m_timeExpires.difftime(t)));
    }
    else  
    {
      target.append("; Expires=", 10);
      m_timeExpires.emitRFCtime(target);
    }
  }

  //a_Next the parameters of this cookie (in order of importance?!?)
  if (!m_strPath.isEmpty())
  {
    target.append("; Path=", 7);
    target.append(m_strPath);
  }

  if (!m_strDomain.isEmpty())
  {
    target.append("; Domain=", 9);
    target.append(m_strDomain);
  }

  if (m_iVersion > 0)
  {
    target.append("; Version=", 10);
    target.append(AString::fromInt(m_iVersion));
  }

  if (!m_strComment.isEmpty())
  {
    target.append("; Comment=", 10);
    target.append(m_strComment);
  }

  //a_Last the SECURE flag
  if (m_boolSecure)
    target.append("; Secure", 8);
  
  target.append("; ", 2);
}

void ACookie::emitRequestHeaderString(AOutputBuffer& target) const
{
  target.append(m_strName);
  target.append(AConstant::ASTRING_EQUALS);
  target.append(m_strValue);
  target.append("; ", 2);
}

void ACookie::setExpires(const ATime &timeExpires) 
{ 
  m_boolExpired = false; 
  m_boolExpirationSet = true; 
  m_timeExpires = timeExpires;
  if (m_iVersion == 1)
  {
    //a_RFC-2109
    long lDiff = (long)m_timeExpires.difftime(ATime());
    if (lDiff > 0x0)
      m_lMaxAge = lDiff;
    else
      m_lMaxAge = 0;
  }
}

void ACookie::setMaxAge(long lMaxAge)
{
  m_lMaxAge = lMaxAge;
  m_boolExpirationSet = true;
  m_timeExpires.setToNow();
  ATime tAddOn;
  tAddOn.set((time_t)lMaxAge);
  m_timeExpires += tAddOn;
}

void ACookie::setValue(const AString &strValue)
{
  m_strValue = strValue;
}

void ACookie::setExpired(bool boolFlag)
{ 
  m_boolExpired = boolFlag; 
}

bool ACookie::isExpired()
{ 
  return m_boolExpired; 
}

void ACookie::setNoExpire()
{ 
  m_boolExpired = false; 
  m_boolExpirationSet = false;
}

void ACookie::setDomain(const AString &strDomain)
{ 
  m_strDomain = strDomain;
}

void ACookie::setPath(const AString &strPath)
{ 
  m_strPath = strPath;
}

void ACookie::setSecure(bool boolSecure)
{ 
  m_boolSecure  = boolSecure;
}

void ACookie::setComment(const AString &strComment)
{ 
  m_strComment = strComment;
  m_iVersion = 1;
}

const AString& ACookie::getName() const
{
  return m_strName;
}

const AString& ACookie::getValue() const
{ 
  return m_strValue; 
}

const ATime& ACookie::getExpires() const
{ 
  return m_timeExpires;
}

const AString& ACookie::getDomain() const
{ 
  return m_strDomain;
}

const AString& ACookie::getPath() const    
{
  return m_strPath;
}

bool ACookie::isSecure() const
{ 
  return m_boolSecure;
}

int ACookie::getVersion() const
{ 
  return m_iVersion;
}

long ACookie::getMaxAge() const
{ 
  return m_lMaxAge;
}

const AString& ACookie::getComment() const
{ 
  return m_strComment;
}

void ACookie::setVersion(int iVersion)
{
  m_iVersion = iVersion;   
}
