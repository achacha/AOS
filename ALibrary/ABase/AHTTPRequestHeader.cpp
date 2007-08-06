#include "pchABase.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AException.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlElement.hpp"
#include "ATextConverter.hpp"

#ifdef __DEBUG_DUMP__
void AHTTPRequestHeader::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AHTTPRequestHeader @ " << std::hex << this << std::dec << ") { " << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mstr_Method=" << mstr_Method << std::endl;
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
  mstr_Method("GET",3)
{
}

AHTTPRequestHeader::AHTTPRequestHeader(const AString &strHeader) :
  AHTTPHeader(),
  mstr_Method("GET",3)
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
  murl_Request = that.murl_Request;
  mcookies_Request = that.mcookies_Request;
}

void AHTTPRequestHeader::clear()
{
  mstr_Method.clear();
  murl_Request.clear();
  mcookies_Request.clear();

  AHTTPHeader::clear();
}

const AString& AHTTPRequestHeader::getMethod() const 
{ 
  return mstr_Method;
}

bool AHTTPRequestHeader::isPOST() const 
{ 
  return mstr_Method.equals("POST",4);
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

void AHTTPRequestHeader::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("AHTTPRequestHeader",18);

  //a_The parent will output it as the initial line
  target.addElement(ASW("method",6), mstr_Method);
  target.addElement(ASW("version",7), mstr_HTTPVersion);
  
  if (mstr_HTTPVersion.equalsNoCase("HTTP/1.1", 8))
  {
    //a_RFC2616 Section 14.23 requires Host: for HTTP/1.1
    if (m_Pairs.find("Host") == m_Pairs.end())
      ATHROW_EX(this, AException::InvalidObject, "HTTP/1.1 requires 'Host:' to contain server requested"); 
  }

  //a_From parent
  MAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    target.addElement((*cit).second.getName(), (*cit).second.getValue());
    ++cit;
  }

  murl_Request.emit(target.addElement(ASW("AUrl",4)));
  mcookies_Request.emit(target.addElement(ASW("ACookies",8)));
}

void AHTTPRequestHeader::emit(AOutputBuffer& target) const
{
  //a_The parent will output it as the initial line
  target.append(mstr_Method);
  target.append(AString::sstr_Space);
  target.append(murl_Request.getPathFileAndQueryString());
  target.append(AString::sstr_Space);
  target.append(mstr_HTTPVersion);
  target.append(AString::sstr_CRLF);

  //a_Parent class will do the body
  AHTTPHeader::emit(target);
  
  //a_Cookies
  if (mcookies_Request.size())
  {
    mcookies_Request.emitRequestHeaderString(target);
  }

  //a_Add the empty line to signify the end
  target.append(AString::sstr_CRLF);
}

void AHTTPRequestHeader::emitProxyHeader(AOutputBuffer& target) const
{
  target.append(mstr_Method);
  target.append(' ');
  murl_Request.emit(target);
  target.append(' ');
  target.append(mstr_HTTPVersion);
  target.append(AString::sstr_CRLF);

  if (mstr_HTTPVersion.equalsNoCase("HTTP/1.1", 8))
  {
    //a_RFC2616 Section 14.23 requires Host: for HTTP/1.1
    if (m_Pairs.find("Host") == m_Pairs.end())
      ATHROW_EX(this, AException::InvalidObject, "HTTP/1.1 requires 'Host:' to contain server requested"); 
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
  if (!isValidMethod())
    ATHROW(this, AException::RequestInvalidMethod);
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
  size_t iP = mstr_LineZero.find(AString::sstr_Space);
  if (iP != AConstant::npos)
  {
    mstr_Method.clear();
    mstr_LineZero.peek(mstr_Method, 0, iP);
  }
  else
    return false;

  if (!isValidMethod())
    return false;

  size_t iP2 = mstr_LineZero.rfind(AString::sstr_Space);
  if (iP2 != AConstant::npos)
  {
    AString str;
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
  if (!_isValidVersion())
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
  //a_Methods are case sensitive upper case as per RFC-2616, not valid for extension types (yet?)
  switch(mstr_Method.at(0))
  {
    case 'C' : return (mstr_Method.compareNoCase(ASW("CONNECT",7)) ? false : true);
    case 'G' : return (mstr_Method.compareNoCase(ASW("GET",3)) ? false : true);
    case 'H' : return (mstr_Method.compareNoCase(ASW("HEAD",4)) ? false : true);
    case 'O' : return (mstr_Method.compareNoCase(ASW("OPTIONS",7)) ? false : true);
    case 'P' : return (mstr_Method.compareNoCase(ASW("POST",4)) && mstr_Method.compareNoCase(ASW("PUT",3)) ? false : true);
    case 'D' : return (mstr_Method.compareNoCase(ASW("DELETE",6)) ? false : true);
    case 'T' : return (mstr_Method.compareNoCase(ASW("TRACE",5)) ? false : true);
  }
    
  return false;
}

bool AHTTPRequestHeader::isValidPath() const
{
  return murl_Request.isValid();
}

bool AHTTPRequestHeader::isHttpPipeliningEnabled() const
{
  return equals(AHTTPHeader::HT_GEN_Connection, ASW("keep-alive",10)) && mstr_HTTPVersion.equals("HTTP/1.1");
}
