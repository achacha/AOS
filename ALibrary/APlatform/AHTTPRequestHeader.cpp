/*
Written by Alex Chachanashvili

$Id: AHTTPRequestHeader.cpp 316 2009-11-06 20:57:09Z achacha $
*/
#include "pchAPlatform.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlElement.hpp"

const AString AHTTPRequestHeader::METHOD_CONNECT("CONNECT",7);
const AString AHTTPRequestHeader::METHOD_GET("GET",3);
const AString AHTTPRequestHeader::METHOD_HEAD("HEAD",4); 
const AString AHTTPRequestHeader::METHOD_OPTIONS("OPTIONS",7);
const AString AHTTPRequestHeader::METHOD_POST("POST",4);
const AString AHTTPRequestHeader::METHOD_PUT("PUT",3);
const AString AHTTPRequestHeader::METHOD_DELETE("DELETE",6); 
const AString AHTTPRequestHeader::METHOD_TRACE("TRACE",5);

void AHTTPRequestHeader::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") { " << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mstr_Method=" << mstr_Method << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_MethodId=" << m_MethodId << std::endl;
  ADebugDumpable::indent(os, indent+1) << "murl_Request=" << std::endl;
  murl_Request.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "  mcookies_Request=" << std::endl;
  mcookies_Request.debugDump(os, indent+2);
  
  AHTTPHeader::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AHTTPRequestHeader::AHTTPRequestHeader() :
  AHTTPHeader(),
  mstr_Method(METHOD_GET),
  m_MethodId(METHOD_ID_GET)
{
}

AHTTPRequestHeader::AHTTPRequestHeader(const AString &strHeader) :
  AHTTPHeader(),
  mstr_Method(METHOD_GET),
  m_MethodId(METHOD_ID_GET)
{
  parse(strHeader);
}

AHTTPRequestHeader::AHTTPRequestHeader(const AHTTPRequestHeader& that) :
  AHTTPHeader(that)
{
  _copy(that);
}

AHTTPRequestHeader& AHTTPRequestHeader::operator=(const AHTTPRequestHeader& that)
{
  if (&that != this)
  {
    clear();
    this->AHTTPHeader::_copy(that);
    _copy(that);
  }
  return *this;
}

AHTTPRequestHeader::~AHTTPRequestHeader()
{
}

void AHTTPRequestHeader::_copy(const AHTTPRequestHeader& that)
{
  mstr_Method.assign(that.mstr_Method);
  m_MethodId = that.m_MethodId;
  murl_Request = that.murl_Request;
  mcookies_Request = that.mcookies_Request;
}

void AHTTPRequestHeader::clear()
{
  mstr_Method.clear();
  m_MethodId = METHOD_ID_UNKNOWN;
  murl_Request.clear();
  mcookies_Request.clear();

  AHTTPHeader::clear();
}

void AHTTPRequestHeader::parseUrl(const AString& str)
{
  murl_Request.parse(str);
  set(AHTTPHeader::HT_REQ_Host, murl_Request.getServer());   //a_Required for HTTP/1.1 and nice to have with earlier versions
}

const AString& AHTTPRequestHeader::getMethod() const 
{ 
  return mstr_Method;
}

AHTTPRequestHeader::METHOD_ID AHTTPRequestHeader::getMethodId() const 
{ 
  return m_MethodId;
}

const AUrl &AHTTPRequestHeader::getUrl() const 
{ 
  return murl_Request;
}

const ACookies &AHTTPRequestHeader::getCookies() const 
{ 
  return mcookies_Request;
}

AUrl &AHTTPRequestHeader::useUrl()
{ 
  return murl_Request; 
}

ACookies &AHTTPRequestHeader::useCookies()
{ 
  return mcookies_Request;
}

AXmlElement& AHTTPRequestHeader::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  //a_The parent will output it as the initial line
  thisRoot.addElement(ASW("method",6)).addData(mstr_Method);
  thisRoot.addElement(ASW("version",7)).addData(mstr_HTTPVersion);
  
  //a_From parent
  for (MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin(); cit != m_Pairs.end(); ++cit)
    thisRoot.addElement((*cit).second.getName()).addData((*cit).second.getValue(), AXmlElement::ENC_CDATADIRECT);

  murl_Request.emitXml(thisRoot.addElement(ASW("url",3)));
  mcookies_Request.emitXml(thisRoot.addElement(ASW("cookies",7)));

  return thisRoot;
}

void AHTTPRequestHeader::emit(AOutputBuffer& target) const
{
  if (mstr_HTTPVersion.equals(AHTTPHeader::HTTP_VERSION_1_1) && !exists(AHTTPHeader::HT_REQ_Host))
  {
    //a_RFC2616 Section 14.23 requires host: for HTTP/1.1
    AASSERT_EX(this, AException::InvalidObject, ASWNL("HTTP/1.1 requires 'host:' to contain server requested")); 
  }

  //a_The parent will output it as the initial line
  target.append(mstr_Method);
  target.append(AConstant::ASTRING_SPACE);
  target.append(murl_Request.getPathFileAndQueryString());
  target.append(AConstant::ASTRING_SPACE);
  target.append(mstr_HTTPVersion);
  target.append(AConstant::ASTRING_CRLF);

  //a_Parent class will do the body
  AHTTPHeader::emit(target);
  
  //a_Cookies
  if (mcookies_Request.size())
    mcookies_Request.emitRequestHeaderString(target);

  //a_Add the empty line to signify the end
  target.append(AConstant::ASTRING_CRLF);
}

void AHTTPRequestHeader::emitProxyHeader(AOutputBuffer& target) const
{
  if (mstr_HTTPVersion.equals(AHTTPHeader::HTTP_VERSION_1_1) && !exists(AHTTPHeader::HT_REQ_Host))
  {
    //a_RFC2616 Section 14.23 requires host: for HTTP/1.1
    AASSERT_EX(this, AException::InvalidObject, ASWNL("HTTP/1.1 requires 'host:' to contain server requested")); 
  }

  target.append(mstr_Method);
  target.append(' ');
  murl_Request.emit(target);
  target.append(' ');
  target.append(mstr_HTTPVersion);
  target.append(AConstant::ASTRING_CRLF);

  //a_Parent class will do the body
  AHTTPHeader::emit(target);

  //a_Cookies
  if (mcookies_Request.size())
    mcookies_Request.emitRequestHeaderString(target);

  //a_Add the empty line to signify the end
  target.append(AConstant::ASTRING_CRLF);
}

void AHTTPRequestHeader::setMethod(const AString &strNewMethod)
{ 
  mstr_Method = strNewMethod;
  m_MethodId = _lookupMethodId(strNewMethod);
}

void AHTTPRequestHeader::parseCookie(const AString &strCookieLine)
{
  mcookies_Request.parse(strCookieLine);
}

bool AHTTPRequestHeader::_parseLineZero()
{
  //a_This is an invalid request, but maybe it is a request being built
  if (mstr_LineZero.isEmpty())
    return false;

  //a_METHOD
  AString str;
  size_t iP = mstr_LineZero.find(AConstant::ASTRING_SPACE);
  if (iP != AConstant::npos)
  {
    mstr_LineZero.peek(str, 0, iP);
    setMethod(str);
  }
  else
    return false;

  if (!isValidMethod())
    return false;

  size_t iP2 = mstr_LineZero.rfind(AConstant::ASTRING_SPACE);
  if (iP2 != AConstant::npos)
  {
    //a_Parse the request path and query string
    str.clear();
    mstr_LineZero.peek(str, iP + 1, iP2 - iP - 1);
    murl_Request.parse(str);
  }
  else
    return false;

  //a_Validate the version
  mstr_HTTPVersion.clear();
  mstr_LineZero.peek(mstr_HTTPVersion, iP2 + 1);
  mstr_HTTPVersion.stripTrailing();
  if (!isValidVersion())
    return false;

  return true;
}

bool AHTTPRequestHeader::_handledByChild(const ANameValuePair &nvHTTPPair)
{
  if (!nvHTTPPair.getName().compare(AHTTPHeader::HT_REQ_Cookie))
  {
    ANameValuePair nvPair(ANameValuePair::COOKIE);

    AString strLine = nvHTTPPair.getValue();
    size_t u4Pos = 0, u4Length = strLine.getSize();
    while (u4Pos < u4Length)
    {
      //a_Get the next item in the line
      nvPair.parse(strLine, u4Pos);

      if (!nvPair.getName().isEmpty())
      {
        mcookies_Request.addCookie(nvPair.getName(), nvPair.getValue());
      }
    }
    return true;
  }
  else
    return false;
}

bool AHTTPRequestHeader::isValidMethod() const
{
  return m_MethodId != METHOD_ID_UNKNOWN;
}

AHTTPRequestHeader::METHOD_ID AHTTPRequestHeader::_lookupMethodId(const AString& method) const
{
  //a_Methods are case sensitive upper case as per RFC-2616
  switch(method.at(0))
  {
    case 'C' : 
      return (method.equals(METHOD_CONNECT) ? METHOD_ID_CONNECT : METHOD_ID_UNKNOWN);

    case 'G' : 
      return (method.equals(METHOD_GET) ? METHOD_ID_GET : METHOD_ID_UNKNOWN);

    case 'H' : 
      return (method.equals(METHOD_HEAD) ? METHOD_ID_HEAD : METHOD_ID_UNKNOWN);

    case 'O' : 
      return (method.equals(METHOD_OPTIONS) ? METHOD_ID_OPTIONS : METHOD_ID_UNKNOWN);

    case 'P' : 
      if (method.equals(METHOD_POST))
        return METHOD_ID_POST;
      else if (method.equals(METHOD_PUT))
        return METHOD_ID_PUT;
      else
        return METHOD_ID_UNKNOWN;

    case 'D' :
      return (method.equals(METHOD_DELETE) ? METHOD_ID_DELETE : METHOD_ID_UNKNOWN);

    case 'T' :
      return (method.equals(METHOD_TRACE) ? METHOD_ID_TRACE : METHOD_ID_UNKNOWN);

    default  :
      return METHOD_ID_UNKNOWN;
  }
}

bool AHTTPRequestHeader::isValidPath() const
{
  return murl_Request.isValid();
}

bool AHTTPRequestHeader::isHttpPipeliningEnabled() const
{
  bool isEnabled = equalsNoCase(AHTTPHeader::HT_GEN_Connection, ASW("keep-alive",10));
  isEnabled |= !equalsNoCase(AHTTPHeader::HT_GEN_Connection, ASW("close",5));
  isEnabled &= mstr_HTTPVersion.equals(AHTTPHeader::HTTP_VERSION_1_1);
  if (m_MethodId == AHTTPRequestHeader::METHOD_ID_POST || m_MethodId == AHTTPRequestHeader::METHOD_ID_PUT)
    isEnabled &= exists(AHTTPHeader::HT_ENT_Content_Length);

  return isEnabled;
}

bool AHTTPRequestHeader::isValidHttp() const
{
  if (mstr_HTTPVersion.equals(AHTTPHeader::HTTP_VERSION_1_1))
  {
    //a_RFC2616 Section 14.23 requires Host: for HTTP/1.1
    if (m_Pairs.find(AHTTPHeader::HT_REQ_Host) == m_Pairs.end())
      return false;
  }
  return true;
}

ATime AHTTPRequestHeader::getIfModifiedSince() const
{
  AString str;
  if (get(AHTTPHeader::HT_REQ_If_Modified_Since, str))
  {
    ATime ret;
    ret.parseRFCtime(str);
    return ret;
  }
  else
    return ATime::EPOCH;
}

bool AHTTPRequestHeader::isFormPost() const
{
  return (AHTTPRequestHeader::METHOD_ID_POST == m_MethodId && equals(AHTTPRequestHeader::HT_ENT_Content_Type, AHTTPRequestHeader::CONTENT_TYPE_HTML_FORM));
}

bool AHTTPRequestHeader::isFormMultiPartPost() const
{
  return (AHTTPRequestHeader::METHOD_ID_POST == m_MethodId && equals(AHTTPRequestHeader::HT_ENT_Content_Type, AHTTPRequestHeader::CONTENT_TYPE_HTML_FORM_MULTIPART));
}

size_t AHTTPRequestHeader::getAcceptLanguageList(LIST_AString& target) const
{
  target.clear();

  AString str;
  if (!get(AHTTPHeader::HT_REQ_Accept_Language, str))
    return 0;

  //a_Split on ,
  LIST_AString tokens;
  str.split(tokens, ',', AConstant::ASTRING_WHITESPACE);
  
  //a_Parse quality
  std::multimap<double, AString> langmap;
  AString strQuality;
  double quality;
  for (LIST_AString::iterator it = tokens.begin(); it != tokens.end(); ++it)
  {
    size_t pos = (*it).find(';');
    if (AConstant::npos != pos)
    {
      //a_Quality found, search to the first number
      strQuality.clear();
      (*it).get(strQuality, pos);
      strQuality.removeUntilOneOf(AConstant::CHARSET_NUMERIC, false);
      if (strQuality.getSize() > 0)
      {
        quality = strQuality.toDouble();
      }
      else
        quality = 1.0;
    }
    else
      quality = 1.0;

    //a_Add lower case version
    it->makeLower();
    langmap.insert(std::multimap<double, AString>::value_type(quality, *it));
  }

  //a_Add to the list
  size_t added = 0;
  for (std::multimap<double, AString>::const_iterator cit = langmap.begin(); cit != langmap.end(); ++cit, ++added)
    target.push_front(cit->second);

  return added;
}

