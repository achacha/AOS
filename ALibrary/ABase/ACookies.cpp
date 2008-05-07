#include "pchABase.hpp"
#include "ACookies.hpp"
#include "ANameValuePair.hpp"
#include "ATextConverter.hpp"
#include "AXmlElement.hpp"

void ACookies::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ACookies @ " << std::hex << this << std::dec << ") { " << std::endl;
  for (size_t u=0; u<m_Cookies.size(); ++u)
  {
    m_Cookies[u]->debugDump(os, indent+1);
  }
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ACookies::ACookies()
{
}

ACookies::ACookies(const AString &strLine)
{
  parse(strLine);
}

ACookies::ACookies(const ACookies &cookiesSource)
{
  __copy(cookiesSource);
}

ACookies &ACookies::operator =(const ACookies& cookiesSource)
{
  __copy(cookiesSource);

  return *this;
}

ACookies::~ACookies()
{
  try
  {
    clear();    //a_Free memory
  }
  catch (...) {}
}

void ACookies::__copy(const ACookies &cookiesSource)
{
  clear();

  for (size_t i=0; i < cookiesSource.m_Cookies.size(); ++i)
  {
    m_Cookies.push_back(new ACookie(*cookiesSource.m_Cookies[i]));
  }
}

void ACookies::clear()
{
  size_t iSize = m_Cookies.size();
  for (size_t iX = 0x0; iX < iSize; iX++)
    pDelete(m_Cookies[iX]);

  m_Cookies.clear();
}

ACookie &ACookies::addCookie(const AString& name, const AString &value)
{
  AASSERT(this, m_Cookies.size() < DEBUG_MAXSIZE_ACache_FileSystem);  //a_Debug only limit

  ACookie *pcookieNew = __findCookie(name);
  if (pcookieNew)
  {
    //a_Already exists, reset and set new values
    pcookieNew->clear();
    pcookieNew->setNameValue(name, value);
  }
  else
  {  
    //a_Will URL encode the value
    pcookieNew = new ACookie(name, value);
    m_Cookies.push_back(pcookieNew);
  }

  return *pcookieNew;
}

bool ACookies::exists(const AString& name) const
{
  ACookie *pcookieNew = __findCookie(name);
  
  return (pcookieNew ? true : false);
}


bool ACookies::getValue(const AString& name, AString& target) const
{
  ACookie *pcookieNew = __findCookie(name);
  if (pcookieNew)
  {
    ATextConverter::decodeURL(pcookieNew->getValue(), target);
    return true;
  }
  else
  {
    return false;
  }
}

bool ACookies::getValue(const AString& name, AString& target, const AString &strDefault) const
{
  ACookie *pcookieNew = __findCookie(name);
  if (pcookieNew)
  {
    ATextConverter::decodeURL(pcookieNew->getValue(), target);
    return true;
  }
  else
  {
    target.append(strDefault);
    return false;
  }
}

bool ACookies::getCookie(const AString& name, ACookie& cookieTarget) const
{
  ACookie *pcookieX = __findCookie(name);
  if (pcookieX)
  {
    cookieTarget = *pcookieX;
    return true;
  }
  else
    return false;
}

void ACookies::parse(const AString &strLine)
{
  //a_Parsing implies that a client has set a cookie for this path and domain
  //a_we just need to parse the name/value pairs
  ANameValuePair nvPair(ANameValuePair::COOKIE);

  size_t pos = 0, length = strLine.getSize();
  int iVersion = 0;
  ACookie *pcookieX = NULL;        //a_Contains the current cookie to work on
  while (pos < length)
  {
    //a_Get the next item in the line
    nvPair.parse(strLine, pos);

    if (nvPair.getName().isEmpty() == FALSE)
    {
      //a_Determine if this is an attribute
      if (nvPair.getName()[0x0] == '$')
      {
        //a_RFC-2109 variables
        //a_Version must be 1, but may be another value (not likely though)
        if (nvPair.isNameNoCase("$VERSION"))
          iVersion = atoi(nvPair.getValue().c_str());
      
        //a_Following parameters can only apply to the cookie that exists/created, ignore if out of sequence
        if (pcookieX)
        {
          //a_These are the only 2 that the browser will send back to the server
          if (nvPair.isNameNoCase("$PATH") || nvPair.isNameNoCase("PATH"))
            pcookieX->setPath(nvPair.getValue());
          else if (nvPair.isNameNoCase("$DOMAIN") || nvPair.isNameNoCase("DOMAIN"))
            pcookieX->setDomain(nvPair.getValue());
        }
      }
      
      pcookieX = __findCookie(nvPair.getName());
      if (NULL == pcookieX)
      {
        //a_Found that name, overwrite
        pcookieX = &addCookie(nvPair.getName(), nvPair.getValue());
      }
      else
        pcookieX->setValue(nvPair.getValue());

      //a_Version is contained once in the line
      pcookieX->setVersion(iVersion);   //a_Propagated through all the cookies
    }
  }
}

void ACookies::parseSetCookieLine(const AString &strLine)
{
  ANameValuePair nvPair(ANameValuePair::COOKIE);

  size_t pos = 0, length = strLine.getSize();
  ACookie *pcookieX = NULL;        //a_Contains the current cookie to work on

  //a_Get the NAME=VALUE for this cookie
  nvPair.parse(strLine, pos);

  //a_Find if the cookie exists, else create it
  pcookieX = __findCookie(nvPair.getName());
  if (NULL == pcookieX)
  {
    //a_Found that name, overwrite
    pcookieX = &addCookie(nvPair.getName(), nvPair.getValue());
	if (!pcookieX)
      return; //a_Unable to create, return...
  }

  AASSERT(this, pcookieX);
  while (pos < length)
  {
    //a_Get the next item in the line
    nvPair.parse(strLine, pos);

    if (!nvPair.getName().isEmpty())
    {
      //a_Determine which attribute is being changed
      if (nvPair.isNameNoCase("PATH"))
        pcookieX->setPath(nvPair.getValue());
      else if (nvPair.isNameNoCase("DOMAIN"))
        pcookieX->setDomain(nvPair.getValue());
      else if (nvPair.isNameNoCase("EXPIRES")) 
      {
        ATime tX;
        tX.parseRFCtime( nvPair.getValue() );
        pcookieX->setExpires(tX);
      }
      else if (nvPair.isNameNoCase("SECURE"))
        pcookieX->setSecure(true);
      else if (nvPair.isNameNoCase("VERSION"))
        pcookieX->setVersion(atoi(nvPair.getValue().c_str()));
      else if (nvPair.isNameNoCase("MAX-AGE"))
        pcookieX->setMaxAge(atol(nvPair.getValue().c_str()));
      else if (nvPair.isNameNoCase("COMMENT"))
        pcookieX->setComment(nvPair.getValue());
    }
  }
}

ACookie *ACookies::__findCookie(const AString& name) const
{
  if (name.isEmpty())
    return NULL;

  for (size_t i = 0x0; i < m_Cookies.size(); ++i)
  {
    if (!m_Cookies[i]->getName().compareNoCase(name))
      return m_Cookies[i];
  }

  return NULL;
}

void ACookies::emitResponseHeaderString(AOutputBuffer& target) const
{
  //a_This is what should be included in the HTTP response header (most common use for this object)
  //a_Format: Set-Cookie: NAME1=VALUE1 <parameters>   (1 cookie per line)
  for (size_t i = 0; i < m_Cookies.size(); ++i)
  {
    target.append("Set-Cookie: ", 12);
    
    m_Cookies[i]->emitResponseHeaderString(target);
    target.append(AConstant::ASTRING_CRLF);
  }
}

void ACookies::emitRequestHeaderString(AOutputBuffer& target) const
{
  target.append("Cookie: ", 8);

  //a_This is what should be included in the HTTP request header
  //a_Format: Cookie: NAME1=VALUE1; NAME2=VALUE2; ...  (many cookies per line)
  for (size_t i=0; i < m_Cookies.size(); ++i)
  {
    m_Cookies[i]->emitRequestHeaderString(target);
  }
  target.append(AConstant::ASTRING_CRLF);
}

void ACookies::expireCookie(const AString& name)
{
  ACookie *pcookieX = __findCookie(name);
  if (!pcookieX)
    pcookieX = &addCookie(name, "");    //a_Add the cookie to the list as expired
  
  if (pcookieX)
    pcookieX->setExpired();
}

void ACookies::removeCookie(const AString& name)
{
  CONTAINER::iterator viCookie = m_Cookies.begin();
  while (viCookie != m_Cookies.end())
  {
    if (!(*viCookie)->getName().compareNoCase(name))
    {
      pDelete(*viCookie);
      m_Cookies.erase(viCookie);
      return;
    }
    viCookie++;
  }
}

void ACookies::emit(AOutputBuffer& target) const 
{ 
  emitRequestHeaderString(target);
}

AXmlElement& ACookies::emitXml(AXmlElement& thisRoot) const 
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  for (size_t i = 0; i < m_Cookies.size(); ++i)
    m_Cookies[i]->emitXml(thisRoot.addElement(ASW("cookie",6)));

  return thisRoot;
}

size_t ACookies::size() const
{ 
  return m_Cookies.size();
}

const ACookies::CONTAINER& ACookies::getContainer() const
{
  return m_Cookies;
}
