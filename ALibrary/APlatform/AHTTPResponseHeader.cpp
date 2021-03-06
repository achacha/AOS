/*
Written by Alex Chachanashvili

$Id: AHTTPResponseHeader.cpp 310 2009-10-16 21:40:38Z achacha $
*/
#include "pchAPlatform.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AXmlElement.hpp"

void AHTTPResponseHeader::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") { " << std::endl;

  ADebugDumpable::indent(os, indent+1) << "mi_StatusCode=" << mi_StatusCode << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mstr_ReasonPhrase=" << mstr_ReasonPhrase << std::endl;

  ADebugDumpable::indent(os, indent+1) << "mcookies_Response=" << std::endl;
  mcookies_Response.debugDump(os, indent+2);

  AHTTPHeader::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AHTTPResponseHeader::AHTTPResponseHeader() : 
  AHTTPHeader(),
  mi_StatusCode(AHTTPResponseHeader::SC_000_Invalid)
{
}

AHTTPResponseHeader::AHTTPResponseHeader(const AString &strHeader) :
  AHTTPHeader()
{
  parse(strHeader);
}

AHTTPResponseHeader::AHTTPResponseHeader(const AHTTPResponseHeader& that) :
  AHTTPHeader(that)
{
  _copy(that);
}

AHTTPResponseHeader AHTTPResponseHeader::operator=(const AHTTPResponseHeader& that)
{
  clear();
  mcookies_Response.clear();
  this->AHTTPHeader::_copy(that);
  _copy(that);

  return *this;
}

AHTTPResponseHeader::~AHTTPResponseHeader()
{
}

void AHTTPResponseHeader::_copy(const AHTTPResponseHeader& that)
{
  mi_StatusCode = that.mi_StatusCode;
  mstr_ReasonPhrase.assign(that.mstr_ReasonPhrase);
  mcookies_Response = that.mcookies_Response;
}

void AHTTPResponseHeader::clear()
{
  mi_StatusCode = AHTTPResponseHeader::SC_000_Invalid;
  mstr_ReasonPhrase.clear();
  mcookies_Response.clear();

  AHTTPHeader::clear();
}

ACookies& AHTTPResponseHeader::useCookies()
{ 
  return mcookies_Response;
}

const ACookies& AHTTPResponseHeader::getCookies() const
{ 
  return mcookies_Response; 
}

int AHTTPResponseHeader::getStatusCode() const
{ 
  return mi_StatusCode;
}

void AHTTPResponseHeader::setStatusCode(int statusCode, const AString& reason)
{ 
  mi_StatusCode = statusCode;

  if (!isValidStatusCode(mi_StatusCode) && reason.isEmpty())
    throw AException(this, AException::InvalidParameter);

  if (!reason.isEmpty())
    mstr_ReasonPhrase.assign(reason);
  else
    mstr_ReasonPhrase.clear();
}

void AHTTPResponseHeader::setStatusCode(AHTTPResponseHeader::STATUS_CODES eStatusCode) 
{ 
  mi_StatusCode = eStatusCode;
  mstr_ReasonPhrase.clear();
}

const AString& AHTTPResponseHeader::getReasonPhrase() const
{
  return mstr_ReasonPhrase;
}

void AHTTPResponseHeader::setReasonPhrase(const AString& reason)
{
  mstr_ReasonPhrase.assign(reason);
}

AXmlElement& AHTTPResponseHeader::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());
  
  thisRoot.addElement(ASW("version",7)).addData(mstr_HTTPVersion);

  //a_The parent will output it as the initial line
  if (isValidStatusCode(mi_StatusCode) && mstr_ReasonPhrase.isEmpty())
    thisRoot.addElement(ASW("status",6)).addData(getStatusCodeReasonPhrase(mi_StatusCode)).addAttribute(ASW("code",4), u4(mi_StatusCode));
  else
    thisRoot.addElement(ASW("status",6)).addData(mstr_ReasonPhrase).addAttribute(ASW("code",4), u4(mi_StatusCode));
  
  //a_From parent
  MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    thisRoot.addElement((*cit).second.getName()).addData((*cit).second.getValue(), AXmlElement::ENC_CDATADIRECT);
    ++cit;
  }

  //a_Cookies if any
  mcookies_Response.emitXml(thisRoot.addElement(ASW("cookies",7)));

  return thisRoot;
}

void AHTTPResponseHeader::emit(AOutputBuffer& target) const
{
  AASSERT(this, mi_StatusCode);

  //a_The parent will output it as the initial line
  target.append(mstr_HTTPVersion);
  target.append(' ');
  target.append(AString::fromInt(mi_StatusCode));
  target.append(' ');
  if (isValidStatusCode(mi_StatusCode) && mstr_ReasonPhrase.isEmpty())
    target.append(getStatusCodeReasonPhrase(mi_StatusCode));
  else
    target.append(mstr_ReasonPhrase);
  target.append(AConstant::ASTRING_CRLF);

  //a_Parent class will do the body
  AHTTPHeader::emit(target);

  //a_Cookies if any
  if (mcookies_Response.size())
    mcookies_Response.emitResponseHeaderString(target);

  //a_Add the empty line to signify the end
  target.append(AConstant::ASTRING_CRLF);
}

bool AHTTPResponseHeader::_parseLineZero()
{
  //a_This is an invalid request, but maybe it is a request being built
  if (mstr_LineZero.isEmpty())
    return false;

  //a_Version first
  size_t iP = mstr_LineZero.find(AConstant::ASTRING_SPACE);
  if (iP != AConstant::npos)
  {
    mstr_HTTPVersion.clear();
    mstr_LineZero.peek(mstr_HTTPVersion, 0, iP);
  }
  else
    return false;

  if (!isValidVersion())
    ATHROW(this, AException::ResponseInvalidVersion);

  //a_Status code
  size_t iP2 = mstr_LineZero.find(AConstant::ASTRING_SPACE, iP + 1);
  if (iP2 != AConstant::npos)
  {
    AString str;
    mstr_LineZero.peek(str, iP + 1, iP2 - iP - 1);
    mi_StatusCode = str.toInt();
  }
  else
    return false;

  //a_If the status code is unknown, save the description
  if (!isValidStatusCode(mi_StatusCode))
  {
    mstr_ReasonPhrase.clear();
    mstr_LineZero.peek(mstr_ReasonPhrase, iP2 + 1);
    mstr_ReasonPhrase.stripTrailing();
  }
  return true;
}

bool AHTTPResponseHeader::_handledByChild(const ANameValuePair &nvPair)
{
  if (!nvPair.getName().compareNoCase(AHTTPHeader::HT_RES_Set_Cookie))
  {
    //a_Special method for this special case
    mcookies_Response.parseSetCookieLine(nvPair.getValue());
    return true;
  }
  else
    return false;
}

bool AHTTPResponseHeader::isValidStatusCode(int statusCode)
{
  switch(statusCode)
  {
    case SC_100_Continue:
    case SC_101_Switching_Protocols:
    case SC_200_Ok:
    case SC_201_Created:
    case SC_202_Accepted:
    case SC_203_Non_Authoritative:
    case SC_204_No_Content:
    case SC_205_Reset_Content:
    case SC_206_Partial_Content:
    case SC_300_Multiple_Choices:
    case SC_301_Moved_Permanently:
    case SC_302_Moved_Temporarily:
    case SC_303_See_Other:
    case SC_304_Not_Modified:
    case SC_305_Use_Proxy:
    case SC_307_Temporary_Redirect:
    case SC_400_Bad_Request:
    case SC_401_Unauthorized:
    case SC_402_Payment_Required:
    case SC_403_Forbidden:
    case SC_404_Not_Found:
    case SC_405_Method_Not_Allowed:
    case SC_406_Not_Acceptable:
    case SC_407_Proxy_Authentication_Required:
    case SC_408_Request_Timeout:
    case SC_409_Conflict:
    case SC_410_Gone:
    case SC_411_Length_Required:
    case SC_412_Precondition_Failed:
    case SC_413_Request_Entity_Too_Large:
    case SC_414_Request_URI_Too_Large:
    case SC_415_Unsupported_Media_Type:
    case SC_416_Requested_Range_Not_Satisfiable:
    case SC_417_Expectation_Failed:
    case SC_500_Internal_Server_Error:
    case SC_501_Not_Implemented:
    case SC_502_Bad_Gateway:
    case SC_503_Service_Unavailable:
    case SC_504_Gateway_Timeout:
    case SC_505_HTTP_Version_Not_Supported:
      return true;
  }
  return false;
}

const AString AHTTPResponseHeader::getStatusCodeReasonPhrase(int statusCode)
{
  switch(statusCode)
  {
    case SC_000_Invalid                         : return "INVALID";
    case SC_100_Continue                        : return "Continue";
    case SC_101_Switching_Protocols             : return "Switching Protocols";
    case SC_200_Ok                              : return "OK";
    case SC_201_Created                         : return "Created";
    case SC_202_Accepted                        : return "Accepted";
    case SC_203_Non_Authoritative               : return "Non-Authoritative Information";
    case SC_204_No_Content                      : return "No Content";
    case SC_205_Reset_Content                   : return "Reset Content";
    case SC_206_Partial_Content                 : return "Partial Content";
    case SC_300_Multiple_Choices                : return "Multiple Choices";
    case SC_301_Moved_Permanently               : return "Moved Permanently";
    case SC_302_Moved_Temporarily               : return "Moved Temporarily";
    case SC_303_See_Other                       : return "See Other";
    case SC_304_Not_Modified                    : return "Not Modified";
    case SC_305_Use_Proxy                       : return "Use Proxy";
    case SC_307_Temporary_Redirect              : return "Temporary Redirect";
    case SC_400_Bad_Request                     : return "Bad Request";
    case SC_401_Unauthorized                    : return "Unauthorized";
    case SC_402_Payment_Required                : return "Payment Required";
    case SC_403_Forbidden                       : return "Forbidden";
    case SC_404_Not_Found                       : return "Not Found";
    case SC_405_Method_Not_Allowed              : return "Method Not Allowed";
    case SC_406_Not_Acceptable                  : return "Not Acceptable";
    case SC_407_Proxy_Authentication_Required   : return "Proxy Authentication Required";
    case SC_408_Request_Timeout                 : return "Request Time-out";
    case SC_409_Conflict                        : return "Conflict";
    case SC_410_Gone                            : return "Gone";
    case SC_411_Length_Required                 : return "Length Required";
    case SC_412_Precondition_Failed             : return "Precondition Failed";
    case SC_413_Request_Entity_Too_Large        : return "Request Entity Too Large";
    case SC_414_Request_URI_Too_Large           : return "Request-URI Too Large";
    case SC_415_Unsupported_Media_Type          : return "Unsupported Media Type";
    case SC_416_Requested_Range_Not_Satisfiable : return "Requested Range Not Satisfiable";
    case SC_417_Expectation_Failed              : return "Expectation Failed";
    case SC_500_Internal_Server_Error           : return "Internal Server Error";
    case SC_501_Not_Implemented                 : return "Not Implemented";
    case SC_502_Bad_Gateway                     : return "Bad Gateway";
    case SC_503_Service_Unavailable             : return "Service Unavailable";
    case SC_504_Gateway_Timeout                 : return "Gateway Time-out";
    case SC_505_HTTP_Version_Not_Supported      : return "HTTP Version not supported";
  }
  
  return AConstant::ASTRING_EMPTY;
}

void AHTTPResponseHeader::setLastModified(const ATime& modified)
{
  AString str;
  modified.emitRFCtime(str);
  set(HT_ENT_Last_Modified, str);
}

bool AHTTPResponseHeader::isHttpPipeliningEnabled() const
{
  bool isEnabled = equalsNoCase(AHTTPHeader::HT_GEN_Connection, ASW("keep-alive",10));
  isEnabled |= !equalsNoCase(AHTTPHeader::HT_GEN_Connection, ASW("close",5));
  isEnabled &= mstr_HTTPVersion.equals(AHTTPHeader::HTTP_VERSION_1_1);
  isEnabled &= exists(AHTTPHeader::HT_ENT_Content_Length);

  return isEnabled;
}
