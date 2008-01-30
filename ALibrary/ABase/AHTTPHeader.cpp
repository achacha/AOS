#include "pchABase.hpp"
#include "AHTTPHeader.hpp"
#include "AException.hpp"
#include "templateAutoPtr.hpp"
#include "AOutputBuffer.hpp"
#include "AFile.hpp"
#include "ARope.hpp"
#include "AThread.hpp"

const AString AHTTPHeader::DEFAULT_HTTP_VERSION("HTTP/1.1",8);

void AHTTPHeader::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AHTTPHeader @ " << std::hex << this << std::dec << ") {" << std::endl;
  MAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    AString str;
    (*cit).second.emit(str);
    ADebugDumpable::indent(os, indent+1) << "first=" << (*cit).first << "  second=" << str << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AHTTPHeader::AHTTPHeader() :
  mstr_HTTPVersion(DEFAULT_HTTP_VERSION)
{
}

AHTTPHeader::AHTTPHeader(const AHTTPHeader& that)
{
  _copy(that);
}

AHTTPHeader& AHTTPHeader::operator=(const AHTTPHeader& that)
{
  m_Pairs.clear();
  _copy(that);
  
  return *this;
}

AHTTPHeader::~AHTTPHeader()
{
}

void AHTTPHeader::_copy(const AHTTPHeader& that)
{
  MAP_AString_NVPair::const_iterator cit = that.m_Pairs.begin();
  while (cit != that.m_Pairs.end())
  {
    m_Pairs.insert(MAP_AString_NVPair::value_type((*cit).first, (*cit).second));
    ++cit;
  }
  mstr_LineZero.assign(that.mstr_LineZero);
  mstr_HTTPVersion.assign(that.mstr_HTTPVersion);
}

void AHTTPHeader::clear()
{
  m_Pairs.clear();
  mstr_LineZero.clear();
  mstr_HTTPVersion.assign(DEFAULT_HTTP_VERSION);
}

void AHTTPHeader::emit(AOutputBuffer& target) const
{
  //a_This emit() only emits the body of the header
  //a_The child class takes care of line zero
  //a_This child classes will call this after they handle the request or response logic
  MAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    (*cit).second.emit(target);
    target.append(AConstant::ASTRING_CRLF);
    ++cit;
  }
}

bool AHTTPHeader::exists(AHTTPHeader::HEADER_TOKENS eToken) const
{
  AString strName = _mapTypeToString(eToken);
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  return (m_Pairs.find(strName) == m_Pairs.end() ? false : true);
}

bool AHTTPHeader::equals(AHTTPHeader::HEADER_TOKENS eToken, const AString& str) const
{
  AString strName = _mapTypeToString(eToken);
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  MAP_AString_NVPair::const_iterator cit = m_Pairs.find(strName);
  if (cit != m_Pairs.end() && !(*cit).second.getValue().compare(str))
  {
    return true;
  }

  return false;
}

bool AHTTPHeader::equalsNoCase(AHTTPHeader::HEADER_TOKENS eToken, const AString& str) const
{
  AString strName = _mapTypeToString(eToken);
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  MAP_AString_NVPair::const_iterator cit = m_Pairs.find(strName);
  if (cit != m_Pairs.end() && !(*cit).second.getValue().compareNoCase(str))
  {
    return true;
  }

  return false;
}

bool AHTTPHeader::getPairValue(AHTTPHeader::HEADER_TOKENS eToken, AString& strDest) const
{
  AString strName = _mapTypeToString(eToken);
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  MAP_AString_NVPair::const_iterator cit = m_Pairs.find(strName);
  if (cit != m_Pairs.end())
  {
    strDest.assign((*cit).second.getValue());
    return true;
  }
  else
  {
    strDest.clear();
    return false;
  }
}

bool AHTTPHeader::getPairValue(const AString& strTokenName, AString& strDest) const
{
  MAP_AString_NVPair::const_iterator cit = m_Pairs.find(strTokenName);
  if (cit != m_Pairs.end())
  {
    strDest.assign((*cit).second.getValue());
    return true;
  }
  else
  {
    strDest.clear();
    return false;
  }
}

void AHTTPHeader::parse(const AString &strHeader)
{
  //a_Nothing to do
  if (strHeader.isEmpty())
    return;
  
  size_t u4Pos = 0;
  size_t u4Length = strHeader.getSize();
  
  //a_Read line zero
  u4Pos = strHeader.find("\n");
  if (u4Pos == AConstant::npos)
  {
    //a_Only 1 line
    mstr_LineZero = strHeader;
    return;
  }
  else
  {
    mstr_LineZero.clear();
    strHeader.peek(mstr_LineZero, 0, u4Pos);
    u4Pos++;
    while ((u4Pos < u4Length) && ( (strHeader[u4Pos] == '\r') || (strHeader[u4Pos] == '\n') ) )
      u4Pos++; //a_Advance beyond the \r\n sequence
  }

  //a_Let request or response handle line zero
  if (!_parseLineZero())
    ATHROW_EX(this, AException::InvalidParameter, ASWNL("Unable to parse line zero"));

  //a_Now read the rest of the header
  ANameValuePair nvpair(ANameValuePair::HTTP);
  while (u4Pos < u4Length)
  {
    nvpair.parse(strHeader, u4Pos);

    if (!nvpair.getName().isEmpty())
    {
      //a_Check to see if the request and response was to handle it
      if (!_handledByChild(nvpair))
      {
        if (m_Pairs.find(nvpair.getName()) != m_Pairs.end())
          ATHROW_EX(this, AException::DataConflict, AString("Duplicate name '",16)+nvpair.getName()+ASW("'",1));

        //a_Child did not handle it, we will
        m_Pairs[nvpair.getName()] = nvpair;
        AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AHTTPHeader);  //a_Debug only limit
      }
    }
  }
}

bool AHTTPHeader::parseLineZero(const AString &strLine)
{
  mstr_LineZero = strLine;
  return _parseLineZero();
}

void AHTTPHeader::parseTokenLine(const AString &strLine)
{
  AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AHTTPHeader);  //a_Debug only limit
  
  ANameValuePair nvpair(ANameValuePair::HTTP);
  size_t u4Pos = 0;
  nvpair.parse(strLine, u4Pos);

  if (!nvpair.getName().isEmpty())
  {
    //a_Check to see if the request and response was to handle it
    if (!_handledByChild(nvpair))
    {
      if (m_Pairs.find(nvpair.getName()) != m_Pairs.end())
        ATHROW_EX(this, AException::DataConflict, nvpair.getName());

      //a_Child did not handle it, we will
      m_Pairs[nvpair.getName()] = nvpair;
    }
  }
}

void AHTTPHeader::setPair(const AString& strName, const AString& strValue)
{
  MAP_AString_NVPair::iterator it = m_Pairs.find(strName);
  if (it != m_Pairs.end())
  {
    //a_Exists
    (*it).second.setValue(strValue);
  }
  else
  {
    AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AHTTPHeader);
    ANameValuePair& nvpair = m_Pairs[strName];
    nvpair.setType(ANameValuePair::HTTP);
    nvpair.setName(strName);
    nvpair.setValue(strValue);
  }
}

void AHTTPHeader::setPair(AHTTPHeader::HEADER_TOKENS eToken, const AString &strValue)
{
  AString strName = _mapTypeToString(eToken);
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  setPair(strName, strValue);
}

void AHTTPHeader::removePair(AHTTPHeader::HEADER_TOKENS eToken)
{
  AString strName = _mapTypeToString(eToken);
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  MAP_AString_NVPair::iterator it = m_Pairs.find(strName);
  if (it != m_Pairs.end())
  {
    m_Pairs.erase(it);
  }
}

const AString AHTTPHeader::_mapTypeToString(AHTTPHeader::HEADER_TOKENS eToken) const
{
  switch (eToken)
  {
    case HT_GEN_Cache_Control       : return ASW("Cache-Control",13);
    case HT_GEN_Connection          : return ASW("Connection",10);
    case HT_GEN_Date                : return ASW("Date",4);
    case HT_GEN_Pragma              : return ASW("Pragma",6);
    case HT_GEN_Transfer_Encoding   : return ASW("Transfer-Encoding",17);
    case HT_GEN_Upgrade             : return ASW("Upgrade",7);
    case HT_GEN_Via                 : return ASW("Via",3);
    case HT_GEN_Keep_Alive          : return ASW("Keep-Alive",10);
    case HT_REQ_Accept              : return ASW("Accept",6);
    case HT_REQ_Accept_Charset      : return ASW("Accept-Charset",14);
    case HT_REQ_Accept_Encoding     : return ASW("Accept-Encoding",15);
    case HT_REQ_Accept_Language     : return ASW("Accept-Language",15);
    case HT_REQ_Accept_Ranges       : return ASW("Accept-Ranges",13);
    case HT_REQ_Authorization       : return ASW("Authorization",13);
    case HT_REQ_From                : return ASW("From",4);
    case HT_REQ_Host                : return ASW("Host",4);
    case HT_REQ_If_Modified_Since   : return ASW("If-Modified-Since",17);
    case HT_REQ_If_Match            : return ASW("If-Match",8);
    case HT_REQ_If_None_Match       : return ASW("If-None-Match",13);
    case HT_REQ_If_Range            : return ASW("If-Range",8);
    case HT_REQ_If_Unmodified_Since : return ASW("If-Unmodified-Since",19);
    case HT_REQ_Max_Forwards        : return ASW("Max-Forwards",12);
    case HT_REQ_Proxy_Authorization : return ASW("Proxy-Authorization",19);
    case HT_REQ_Range               : return ASW("Range",5);
    case HT_REQ_Referer             : return ASW("Referer",7);
    case HT_REQ_User_Agent          : return ASW("User-Agent",10);
    case HT_RES_Age                 : return ASW("Age",3);
    case HT_RES_Location            : return ASW("Location",8);
    case HT_RES_Proxy_Authenticate  : return ASW("Proxy-Authenticate",18);
    case HT_RES_Public              : return ASW("Public",6);
    case HT_RES_Retry_After         : return ASW("Retry-After",11);
    case HT_RES_Server              : return ASW("Server",6);
    case HT_RES_Vary                : return ASW("Vary",4);
    case HT_RES_Warning             : return ASW("Warning",7);
    case HT_RES_WWW_Authenticate    : return ASW("WWW-Authenticate",16);
    case HT_RES_TE                  : return ASW("TE",2);
    case HT_ENT_Allow               : return ASW("Allow",5);
    case HT_ENT_Content_Base        : return ASW("Content-Base",12);
    case HT_ENT_Content_Encoding    : return ASW("Content-Encoding",16);
    case HT_ENT_Content_Language    : return ASW("Content-Language",16);
    case HT_ENT_Content_Length      : return ASW("Content-Length",14);
    case HT_ENT_Content_Location    : return ASW("Content-Location",16);
    case HT_ENT_Content_MD5         : return ASW("Content-MD5",11);
    case HT_ENT_Content_Range       : return ASW("Content-Range",13);
    case HT_ENT_Content_Type        : return ASW("Content-Type",12);
    case HT_ENT_ETag                : return ASW("ETag",4);
    case HT_ENT_Expires             : return ASW("Expires",7);
    case HT_ENT_Last_Modified       : return ASW("Last-Modified",13);
  }

  return AConstant::ASTRING_EMPTY;
}

AHTTPHeader::HEADER_TOKENS AHTTPHeader::_mapStringToType(const AString &strToken) const
{
  //a_Very frequent (mixed)
  if (!strToken.compareNoCase(ASW("Content-Type",12)))        return HT_ENT_Content_Type;
  if (!strToken.compareNoCase(ASW("Content-Length",14)))      return HT_ENT_Content_Length;
  if (!strToken.compareNoCase(ASW("Content-Encoding",16)))    return HT_ENT_Content_Encoding;
  if (!strToken.compareNoCase(ASW("Authorization",13)))       return HT_REQ_Authorization;
  if (!strToken.compareNoCase(ASW("User-Agent",10)))          return HT_REQ_User_Agent;
  if (!strToken.compareNoCase(ASW("Referer",7)))              return HT_REQ_Referer;
  if (!strToken.compareNoCase(ASW("WWW-Authenticate",16)))    return HT_RES_WWW_Authenticate;
  if (!strToken.compareNoCase(ASW("Connection",10)))          return HT_GEN_Connection;
  if (!strToken.compareNoCase(ASW("Host",4)))                 return HT_REQ_Host;
  if (!strToken.compareNoCase(ASW("Keep-Alive",10)))          return HT_GEN_Keep_Alive;

  //a_Entity                                                  
  if (!strToken.compareNoCase(ASW("Allow",5)))                return HT_ENT_Allow;
  if (!strToken.compareNoCase(ASW("Content-Base",12)))        return HT_ENT_Content_Base;
  if (!strToken.compareNoCase(ASW("Content-Language",16)))    return HT_ENT_Content_Language;
  if (!strToken.compareNoCase(ASW("Content-Location",16)))    return HT_ENT_Content_Location;
  if (!strToken.compareNoCase(ASW("Content-MD5",11)))         return HT_ENT_Content_MD5;
  if (!strToken.compareNoCase(ASW("Content-Range",13)))       return HT_ENT_Content_Range;
  if (!strToken.compareNoCase(ASW("ETag",4)))                 return HT_ENT_ETag;
  if (!strToken.compareNoCase(ASW("Expires",7)))              return HT_ENT_Expires;
  if (!strToken.compareNoCase(ASW("Last-Modified",13)))       return HT_ENT_Last_Modified;

  //a_Request
  if (!strToken.compareNoCase(ASW("Accept",6)))               return HT_REQ_Accept;
  if (!strToken.compareNoCase(ASW("Accept-Charset",14)))      return HT_REQ_Accept_Charset;
  if (!strToken.compareNoCase(ASW("Accept-Encoding",15)))     return HT_REQ_Accept_Encoding;
  if (!strToken.compareNoCase(ASW("Accept-Language",15)))     return HT_REQ_Accept_Language;
  if (!strToken.compareNoCase(ASW("Accept-Ranges",13)))       return HT_REQ_Accept_Ranges;
  if (!strToken.compareNoCase(ASW("From",4)))                 return HT_REQ_From;
  if (!strToken.compareNoCase(ASW("If-Modified-Since",17)))   return HT_REQ_If_Modified_Since;
  if (!strToken.compareNoCase(ASW("If-Match",8)))             return HT_REQ_If_Match;
  if (!strToken.compareNoCase(ASW("If-None-Match",13)))       return HT_REQ_If_None_Match;
  if (!strToken.compareNoCase(ASW("If-Range",8)))             return HT_REQ_If_Range;
  if (!strToken.compareNoCase(ASW("If-Unmodified-Since",19))) return HT_REQ_If_Unmodified_Since;
  if (!strToken.compareNoCase(ASW("Max-Forwards",12)))        return HT_REQ_Max_Forwards;
  if (!strToken.compareNoCase(ASW("Proxy-Authorization",19))) return HT_REQ_Proxy_Authorization;
  if (!strToken.compareNoCase(ASW("Range",5)))                return HT_REQ_Range;

  //a_Response
  if (!strToken.compareNoCase(ASW("Age",3)))                  return HT_RES_Age;
  if (!strToken.compareNoCase(ASW("Location",8)))             return HT_RES_Location;
  if (!strToken.compareNoCase(ASW("Proxy-Authenticate",18)))  return HT_RES_Proxy_Authenticate;
  if (!strToken.compareNoCase(ASW("Public",6)))               return HT_RES_Public;
  if (!strToken.compareNoCase(ASW("Retry-After",11)))         return HT_RES_Retry_After;
  if (!strToken.compareNoCase(ASW("Server",6)))               return HT_RES_Server;
  if (!strToken.compareNoCase(ASW("Vary",4)))                 return HT_RES_Vary;
  if (!strToken.compareNoCase(ASW("Warning",7)))              return HT_RES_Warning;
  if (!strToken.compareNoCase(ASW("TE",2)))                   return HT_RES_TE;

  //a_General                                                          
  if (!strToken.compareNoCase(ASW("Cache-Control",13)))       return HT_GEN_Cache_Control;
  if (!strToken.compareNoCase(ASW("Date",4)))                 return HT_GEN_Date;
  if (!strToken.compareNoCase(ASW("Pragma",6)))               return HT_GEN_Pragma;
  if (!strToken.compareNoCase(ASW("Transfer-Encoding",17)))   return HT_GEN_Transfer_Encoding;
  if (!strToken.compareNoCase(ASW("Upgrade",7)))              return HT_GEN_Upgrade;
  if (!strToken.compareNoCase(ASW("Via",3)))                  return HT_GEN_Via;

  ATHROW(this, AException::InvalidToken);
}

void AHTTPHeader::setVersion(const AString& version)
{
  mstr_HTTPVersion.assign(version);
  if (!isValidVersion())
    ATHROW(this, AException::InvalidParameter);
}

bool AHTTPHeader::isValidVersion()
{
  //a_So far these 3 versions of HTTP are supported, RFC 2616 is for as of now latest HTTP/1.1
  //a_Case sensitive compare
  if (
       (mstr_HTTPVersion == "HTTP/0.9") ||
       (mstr_HTTPVersion == "HTTP/1.0") ||
       (mstr_HTTPVersion == "HTTP/1.1")
     )
    return true;
  else
    return false;
}


void AHTTPHeader::toAFile(AFile& aFile) const
{
  ARope rope;
  emit(rope);
  aFile.write(rope);
}

void AHTTPHeader::fromAFile(AFile& aFile)
{
  //a_Very simple routine for reading HTTP header, waits indefinitely for lines
  AString str(1024, 1024);
  size_t bytesRead = aFile.readLine(str, AConstant::npos, false);
  if (AConstant::npos == bytesRead)
    return;

  while (AConstant::unavail == bytesRead && aFile.isNotEof())
  {
    AThread::sleep(100);
    bytesRead = aFile.readLine(str, AConstant::npos, false);
  }

  if (bytesRead > 0)
  {
    if (!parseLineZero(str))
    {
      bytesRead = aFile.readLine(str, AConstant::npos, false);
      ATHROW_EX(this, AException::InvalidData, ASWNL("Unable to parse line zero"));
    }

    str.assign(".");
    while (!str.isEmpty())
    {
      str.clear();
      bytesRead = aFile.readLine(str, AConstant::npos, false);
      while (AConstant::npos == bytesRead && aFile.isNotEof())
      {
        AThread::sleep(100);
        bytesRead = aFile.readLine(str, AConstant::npos, false);
      }
      if (str.getSize() > 0)
        parseTokenLine(str);
      
    }
  }
  else
  {
    //a_Either first line is not valid or non-blokcing socket is used
    //a_For non-blocking sockets HTTP header reads must be written by used, this function assumes blocking
    ATHROW_EX(this, AException::InsuffiecientData, ASWNL("Unable to read first line of HTTP header, may be a result of using non-blocking socket"));
  }
}

size_t AHTTPHeader::getKeepAliveTimeout() const
{
  AString str;
  if (getPairValue(AHTTPHeader::HT_GEN_Keep_Alive, str))
    return str.toSize_t();
  else
    return AConstant::npos;
}

const AString& AHTTPHeader::getVersion() const
{ 
  return mstr_HTTPVersion;
}
