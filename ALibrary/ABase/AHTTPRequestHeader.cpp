#include "pchABase.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AException.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlElement.hpp"
#include "ATextConverter.hpp"

const AString AHTTPRequestHeader::METHOD_CONNECT("CONNECT",7);
const AString AHTTPRequestHeader::METHOD_GET("GET",3);
const AString AHTTPRequestHeader::METHOD_HEAD("HEAD",4); 
const AString AHTTPRequestHeader::METHOD_OPTIONS("OPTIONS",7);
const AString AHTTPRequestHeader::METHOD_POST("POST",4);
const AString AHTTPRequestHeader::METHOD_PUT("PUT",3);
const AString AHTTPRequestHeader::METHOD_DELETE("DELETE",6); 
const AString AHTTPRequestHeader::METHOD_TRACE("TRACE",5);

#ifdef __DEBUG_DUMP__
void AHTTPRequestHeader::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AHTTPRequestHeader @ " << std::hex << this << std::dec << ") { " << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mstr_Method=" << mstr_Method << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_MethodId=" << m_MethodId << std::endl;
  ADebugDumpable::indent(os, indent+1) << "murl_Request=" << std::endl;
  murl_Request.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "  mcookies_Request=" << std::endl;
  mcookies_Request.debugDump(os, indent+2);
  
  AHTTPHeader::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

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
  setPair(AHTTPHeader::HT_REQ_Host, murl_Request.getServer());   //a_Required for HTTP/1.1 and nice to have with earlier versions
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

void AHTTPRequestHeader::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("AHTTPRequestHeader",18);

  //a_The parent will output it as the initial line
  target.addElement(ASW("method",6), mstr_Method);
  target.addElement(ASW("version",7), mstr_HTTPVersion);
  
  //a_From parent
  MAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    target.addElement((*cit).second.getName(), (*cit).second.getValue());
    ++cit;
  }

  murl_Request.emitXml(target.addElement(ASW("AUrl",4)));
  mcookies_Request.emitXml(target.addElement(ASW("ACookies",8)));
}

void AHTTPRequestHeader::emit(AOutputBuffer& target) const
{
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
  {
    mcookies_Request.emitRequestHeaderString(target);
  }

  //a_Add the empty line to signify the end
  target.append(AConstant::ASTRING_CRLF);
}

void AHTTPRequestHeader::emitProxyHeader(AOutputBuffer& target) const
{
  target.append(mstr_Method);
  target.append(' ');
  murl_Request.emit(target);
  target.append(' ');
  target.append(mstr_HTTPVersion);
  target.append(AConstant::ASTRING_CRLF);

  if (mstr_HTTPVersion.equalsNoCase("HTTP/1.1", 8))
  {
    //a_RFC2616 Section 14.23 requires Host: for HTTP/1.1
    if (m_Pairs.find("Host") == m_Pairs.end())
      ATHROW_EX(this, AException::InvalidObject, ASWNL("HTTP/1.1 requires 'Host:' to contain server requested")); 
  }

  //a_Parent class will do the body
  AHTTPHeader::emit(target);

  //a_Cookies
  if (mcookies_Request.size())
    mcookies_Request.emitRequestHeaderString(target);
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
    str.clear();
    mstr_LineZero.peek(str, iP + 1, iP2 - iP - 1);
    
    AString strDecoded(str.getSize() * 2, 256);
    ATextConverter::decodeURL(str, strDecoded);
    murl_Request.parse(strDecoded);
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
  static const AString STR_COOKIE("Cookie", 6);
  if (!nvHTTPPair.getName().compareNoCase(STR_COOKIE))
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
  return equals(AHTTPHeader::HT_GEN_Connection, ASW("keep-alive",10)) && mstr_HTTPVersion.equals("HTTP/1.1", 8);
}

bool AHTTPRequestHeader::isValidHttp() const
{
  if (mstr_HTTPVersion.equalsNoCase("HTTP/1.1", 8))
  {
    //a_RFC2616 Section 14.23 requires Host: for HTTP/1.1
    if (m_Pairs.find(ASW("Host",4)) == m_Pairs.end())
      return false;
  }
  return true;
}

ATime AHTTPRequestHeader::getIfModifiedSince() const
{
  AString str;
  if (getPairValue(AHTTPHeader::HT_REQ_If_Modified_Since, str))
  {
    ATime ret;
    ret.parseRFCtime(str);
    return ret;
  }
  else
    return ATime::GENESIS;
}
