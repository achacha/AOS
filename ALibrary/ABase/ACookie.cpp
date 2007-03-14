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
  ADebugDumpable::indent(os, indent+1) << "m__strName=" << m__strName << std::endl;   
  ADebugDumpable::indent(os, indent+1) << "m__strValue=" << m__strValue << std::endl;  
  ADebugDumpable::indent(os, indent+1) << "m__strPath=" << m__strPath << std::endl;   
  ADebugDumpable::indent(os, indent+1) << "m__strDomain=" << m__strDomain << std::endl; 
  ADebugDumpable::indent(os, indent+1) << "m__boolSecure=" << m__boolSecure << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__boolExpired=" << m__boolExpired << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__boolExpirationSet=" << m__boolExpirationSet << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__iVersion=" << m__iVersion << std::endl;  
  ADebugDumpable::indent(os, indent+1) << "m__strComment=" << m__strComment << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__timeExpires={";
  m__timeExpires.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) <<"}" << std::endl;
  
  ADebugDumpable::indent(os, indent) <<"}" << std::endl;
}
#endif

ACookie::ACookie() :
  m__boolExpired(false),
  m__iVersion(0x0),           //a_Default to "old Netscape" style cookies
  m__boolSecure(false),
  m__boolExpirationSet(false) //a_Indefinite cookie
{
}

ACookie::ACookie(const AString &strName, const AString &strValue) :
  m__boolExpired(false),
  m__iVersion(0x0),           //a_Default to "old Netscape" style cookies
  m__boolSecure(false),
  m__strName(strName),
  m__strValue(strValue),
  m__boolExpirationSet(false) //a_Indefinite cookie
{
}

void ACookie::setNameValue(const AString &strName, const AString &strValue)
{
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  m__strName = strName;
  m__strValue = strValue;
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
  m__strName           = cookieSource.m__strName;   
  m__strValue          = cookieSource.m__strValue;  
  m__strPath           = cookieSource.m__strPath;   
  m__strDomain         = cookieSource.m__strDomain; 
  m__timeExpires       = cookieSource.m__timeExpires;
  m__boolSecure        = cookieSource.m__boolSecure;
                            
  m__boolExpired       = cookieSource.m__boolExpired;
  m__boolExpirationSet = cookieSource.m__boolExpirationSet;
 
  m__iVersion          = cookieSource.m__iVersion;  
  m__strComment        = cookieSource.m__strComment;
}

//a_Used before overlaying the cookie (faster than delete/new)
void ACookie::clear()
{
  m__strName.clear();          
  m__strValue.clear();         
  m__strPath.clear();          
  m__strDomain.clear();        
  m__timeExpires.setToNow();
  m__boolSecure = false;      
                      
  m__boolExpired       = false;
  m__boolExpirationSet = false;
 
  m__iVersion = 0x0;         
  m__strComment.clear();       
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

void ACookie::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("ACookie",7);
  
  target.addElement(ASW("name",4), m__strName);
  target.addElement(ASW("value",5), m__strValue);
  target.addElement(ASW("domain",6), m__strDomain);
  target.addElement(ASW("path",4), m__strPath);
  target.addElement(ASW("version",7), u4(m__iVersion));
  target.addElement(ASW("comment",7), m__strComment);
  target.addElement(ASW("maxage",6), u4(m__lMaxAge));
  
  AString str;
  m__timeExpires.emitRFCtime(str);
  target.addElement(ASW("expires",7), str);
  
  if (m__boolSecure) target.addElement(ASW("secure",6));
}

void ACookie::emitResponseHeaderString(AOutputBuffer& target) const
{
  //a_First the NAME=VALUE must be set; "=" is actually optional, but netscape needs it (due to misimplementation)
  target.append(m__strName);
  target.append(AString::sstr_Equals);
  target.append(m__strValue);

  //a_Now let's figure out the expiration
  if (m__boolExpired)
  {
    //a_Cookie is to be expired on the client
    if (m__iVersion == 1)
      target.append("; MAX-AGE=0", 11);
    else
       target.append("; EXPIRES=Tue, 01-Jan-1980 00:00:01 UT", 38);   //a_This assumes no abberations of the time-space conitnuum
  }
  else if (m__boolExpirationSet)
  {
    //a_Expiration was set at some point, use it, else no EXPIRES is needed
    if (m__iVersion == 1)
    {
      ATime t;    //a_Time now

      target.append("; MAX-AGE=", 10);
      target.append(AString::fromS4((s4)m__timeExpires.difftime(t)));
    }
    else  
      target.append("; EXPIRES=", 10);
      m__timeExpires.emitRFCtime(target);
  }

  //a_Next the parameters of this cookie (in order of importance?!?)
  if (!m__strPath.isEmpty())
  {
    target.append("; PATH=", 7);
    target.append(m__strPath);
  }

  if (!m__strDomain.isEmpty())
  {
    target.append("; DOMAIN=", 9);
    target.append(m__strDomain);
  }

  if (m__iVersion > 0)
  {
    target.append("; VERSION=", 10);
    target.append(AString::fromInt(m__iVersion));
  }

  if (!m__strComment.isEmpty())
  {
    target.append("; COMMENT=", 10);
    target.append(m__strComment);
  }

  //a_Last the SECURE flag
  if (m__boolSecure)
    target.append("; SECURE", 8);
  
  target.append("; ", 2);
}

void ACookie::emitRequestHeaderString(AOutputBuffer& target) const
{
  target.append(m__strName);
  target.append(AString::sstr_Equals);
  target.append(m__strValue);
  target.append("; ", 2);
}

void ACookie::setExpires(const ATime &timeExpires) 
{ 
  m__boolExpired = false; 
  m__boolExpirationSet = true; 
  m__timeExpires = timeExpires;
  if (m__iVersion == 1)
  {
    //a_RFC-2109
    long lDiff = (long)m__timeExpires.difftime(ATime());
    if (lDiff > 0x0)
      m__lMaxAge = lDiff;
    else
      m__lMaxAge = 0;
  }
}

void ACookie::setMaxAge(long lMaxAge)
{
  m__lMaxAge = lMaxAge;
  m__boolExpirationSet = true;
  m__timeExpires.setToNow();
  ATime tAddOn;
  tAddOn.set((time_t)lMaxAge);
  m__timeExpires += tAddOn;
}

int ACookie::compare(ACookie&) const
{
//TODO: 
#pragma message("Need to implement comparisson for ACookie")
ATHROW(this, AException::NotImplemented);
}
