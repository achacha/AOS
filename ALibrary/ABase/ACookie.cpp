#include "pchABase.hpp"
#include "ACookie.hpp"
#include "AException.hpp"
#include "ATextConverter.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
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
#endif

ACookie::ACookie() :
  m_boolExpired(false),
  m_iVersion(1),
  m_boolSecure(false),
  m_boolExpirationSet(false), //a_Indefinite cookie
  m_lMaxAge(-1)
{
}

ACookie::ACookie(const AString &strName, const AString &strValue) :
  m_boolExpired(false),
  m_iVersion(1),
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
 
  m_iVersion = 1;
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

void ACookie::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("ACookie",7);
  
  target.addElement(ASW("name",4), m_strName);
  target.addElement(ASW("value",5), m_strValue);
  target.addElement(ASW("domain",6), m_strDomain);
  target.addElement(ASW("path",4), m_strPath);
  target.addElement(ASW("version",7)).addData(m_iVersion);
  target.addElement(ASW("comment",7), m_strComment);
  target.addElement(ASW("maxage",6)).addData(m_lMaxAge);
  
  AString str;
  m_timeExpires.emitRFCtime(str);
  target.addElement(ASW("expires",7), str);
  
  if (m_boolSecure) target.addElement(ASW("secure",6));
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
      target.append("; MAX-AGE=0", 11);
    else
       target.append("; EXPIRES=Tue, 01-Jan-1980 00:00:01 UT", 38);   //a_This assumes no abberations of the time-space conitnuum
  }
  else if (m_boolExpirationSet)
  {
    //a_Expiration was set at some point, use it, else no EXPIRES is needed
    if (m_iVersion == 1)
    {
      ATime t;    //a_Time now

      target.append("; MAX-AGE=", 10);
      if (-1 != m_lMaxAge)
        target.append(AString::fromS4(m_lMaxAge));
      else
        target.append(AString::fromS4((s4)m_timeExpires.difftime(t)));
    }
    else  
      target.append("; EXPIRES=", 10);
      m_timeExpires.emitRFCtime(target);
  }

  //a_Next the parameters of this cookie (in order of importance?!?)
  if (!m_strPath.isEmpty())
  {
    target.append("; PATH=", 7);
    target.append(m_strPath);
  }

  if (!m_strDomain.isEmpty())
  {
    target.append("; DOMAIN=", 9);
    target.append(m_strDomain);
  }

  if (m_iVersion > 0)
  {
    target.append("; VERSION=", 10);
    target.append(AString::fromInt(m_iVersion));
  }

  if (!m_strComment.isEmpty())
  {
    target.append("; COMMENT=", 10);
    target.append(m_strComment);
  }

  //a_Last the SECURE flag
  if (m_boolSecure)
    target.append("; SECURE", 8);
  
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

int ACookie::compare(ACookie&) const
{
//TODO: 
#pragma message("Need to implement comparisson for ACookie")
ATHROW(this, AException::NotImplemented);
}
