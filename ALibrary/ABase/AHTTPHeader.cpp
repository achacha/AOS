/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AHTTPHeader.hpp"
#include "AOutputBuffer.hpp"
#include "AFile.hpp"
#include "ARope.hpp"
#include "AThread.hpp"

const AString AHTTPHeader::HTTP_VERSION_0_9("HTTP/0.9",8);
const AString AHTTPHeader::HTTP_VERSION_1_0("HTTP/1.0",8);
const AString AHTTPHeader::HTTP_VERSION_1_1("HTTP/1.1",8);

const AString AHTTPHeader::CONTENT_TYPE_HTML_FORM("application/x-www-form-urlencoded");
const AString AHTTPHeader::CONTENT_TYPE_HTML_FORM_MULTIPART("multipart/form-data");

const AString AHTTPHeader::HT_GEN_Cache_Control      ("cache-control",13);
const AString AHTTPHeader::HT_GEN_Connection         ("connection",10);
const AString AHTTPHeader::HT_GEN_Date               ("date",4);
const AString AHTTPHeader::HT_GEN_Pragma             ("pragma",6);
const AString AHTTPHeader::HT_GEN_Transfer_Encoding  ("transfer-encoding",17);
const AString AHTTPHeader::HT_GEN_Upgrade            ("upgrade",7);
const AString AHTTPHeader::HT_GEN_Via                ("via",3);
const AString AHTTPHeader::HT_GEN_Keep_Alive         ("keep-alive",10);
const AString AHTTPHeader::HT_REQ_Accept             ("accept",6);
const AString AHTTPHeader::HT_REQ_Accept_Charset     ("accept-charset",14);
const AString AHTTPHeader::HT_REQ_Accept_Encoding    ("accept-encoding",15);
const AString AHTTPHeader::HT_REQ_Accept_Language    ("accept-language",15);
const AString AHTTPHeader::HT_REQ_Accept_Ranges      ("accept-ranges",13);
const AString AHTTPHeader::HT_REQ_Authorization      ("authorization",13);
const AString AHTTPHeader::HT_REQ_From               ("from",4);
const AString AHTTPHeader::HT_REQ_Host               ("host",4);
const AString AHTTPHeader::HT_REQ_If_Modified_Since  ("if-modified-since",17);
const AString AHTTPHeader::HT_REQ_If_Match           ("if-match",8);
const AString AHTTPHeader::HT_REQ_If_None_Match      ("if-none-match",13);
const AString AHTTPHeader::HT_REQ_If_Range           ("if-range",8);
const AString AHTTPHeader::HT_REQ_If_Unmodified_Since("if-unmodified-since",19);
const AString AHTTPHeader::HT_REQ_Max_Forwards       ("max-forwards",12);
const AString AHTTPHeader::HT_REQ_Proxy_Authorization("proxy-authorization",19);
const AString AHTTPHeader::HT_REQ_Range              ("range",5);
const AString AHTTPHeader::HT_REQ_Referer            ("referer",7);
const AString AHTTPHeader::HT_REQ_User_Agent         ("user-agent",10);
const AString AHTTPHeader::HT_REQ_Cookie             ("cookie",6);
const AString AHTTPHeader::HT_RES_Age                ("age",3);
const AString AHTTPHeader::HT_RES_Location           ("location",8);
const AString AHTTPHeader::HT_RES_Proxy_Authenticate ("proxy-authenticate",18);
const AString AHTTPHeader::HT_RES_Public             ("public",6);
const AString AHTTPHeader::HT_RES_Retry_After        ("retry-after",11);
const AString AHTTPHeader::HT_RES_Server             ("server",6);
const AString AHTTPHeader::HT_RES_Vary               ("vary",4);
const AString AHTTPHeader::HT_RES_Warning            ("warning",7);
const AString AHTTPHeader::HT_RES_WWW_Authenticate   ("www-authenticate",16);
const AString AHTTPHeader::HT_RES_TE                 ("te",2);
const AString AHTTPHeader::HT_RES_Set_Cookie         ("set-cookie",10);
const AString AHTTPHeader::HT_ENT_Allow              ("allow",5);
const AString AHTTPHeader::HT_ENT_Content_Base       ("content-base",12);
const AString AHTTPHeader::HT_ENT_Content_Encoding   ("content-encoding",16);
const AString AHTTPHeader::HT_ENT_Content_Language   ("content-language",16);
const AString AHTTPHeader::HT_ENT_Content_Length     ("content-length",14);
const AString AHTTPHeader::HT_ENT_Content_Location   ("content-location",16);
const AString AHTTPHeader::HT_ENT_Content_MD5        ("content-md5",11);
const AString AHTTPHeader::HT_ENT_Content_Range      ("content-range",13);
const AString AHTTPHeader::HT_ENT_Content_Type       ("content-type",12);
const AString AHTTPHeader::HT_ENT_ETag               ("etag",4);
const AString AHTTPHeader::HT_ENT_Expires            ("expires",7);
const AString AHTTPHeader::HT_ENT_Last_Modified      ("last-modified",13);

void AHTTPHeader::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
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
  mstr_HTTPVersion(HTTP_VERSION_1_1)
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
  MMAP_AString_NVPair::const_iterator cit = that.m_Pairs.begin();
  while (cit != that.m_Pairs.end())
  {
    m_Pairs.insert(MMAP_AString_NVPair::value_type((*cit).first, (*cit).second));
    ++cit;
  }
  mstr_LineZero.assign(that.mstr_LineZero);
  mstr_HTTPVersion.assign(that.mstr_HTTPVersion);
}

void AHTTPHeader::clear()
{
  m_Pairs.clear();
  mstr_LineZero.clear();
  mstr_HTTPVersion.assign(HTTP_VERSION_1_1);
}

void AHTTPHeader::emit(AOutputBuffer& target) const
{
  //a_This emit() only emits the body of the header
  //a_The child class takes care of line zero
  //a_This child classes will call this after they handle the request or response logic
  MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    (*cit).second.emit(target);
    target.append(AConstant::ASTRING_CRLF);
    ++cit;
  }
}

bool AHTTPHeader::exists(const AString& name) const
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  return (m_Pairs.find(name) == m_Pairs.end() ? false : true);
}

size_t AHTTPHeader::count(const AString& name) const
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  return m_Pairs.count(name);
}

bool AHTTPHeader::equals(const AString& name, const AString& value) const
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  std::pair<MMAP_AString_NVPair::const_iterator, MMAP_AString_NVPair::const_iterator> citPairs = m_Pairs.equal_range(name);
  for(MMAP_AString_NVPair::const_iterator cit = citPairs.first; cit != citPairs.second; ++cit)
    if ((*cit).second.getValue().equals(value))
      return true;

  return false;
}

bool AHTTPHeader::equalsNoCase(const AString& name, const AString& value) const
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  std::pair<MMAP_AString_NVPair::const_iterator, MMAP_AString_NVPair::const_iterator> citPairs = m_Pairs.equal_range(name);
  for(MMAP_AString_NVPair::const_iterator cit = citPairs.first; cit != citPairs.second; ++cit)
    if ((*cit).second.getValue().equalsNoCase(value))
      return true;

  return false;
}

size_t AHTTPHeader::get(const AString& name, AOutputBuffer& target) const
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  size_t added = 0;
  std::pair<MMAP_AString_NVPair::const_iterator, MMAP_AString_NVPair::const_iterator> citPairs = m_Pairs.equal_range(name);
  for(MMAP_AString_NVPair::const_iterator cit = citPairs.first; cit != citPairs.second; ++added)
  {
    target.append((*cit).second.getValue());
    ++cit;
    if (cit != citPairs.second)
      target.append(',');
  }

  return added;
}

size_t AHTTPHeader::get(const AString& name, LIST_AString& target) const
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  size_t added = 0;
  std::pair<MMAP_AString_NVPair::const_iterator, MMAP_AString_NVPair::const_iterator> citPairs = m_Pairs.equal_range(name);
  for(MMAP_AString_NVPair::const_iterator cit = citPairs.first; cit != citPairs.second; ++cit, ++added)
    target.push_back((*cit).second.getValue());
  
  return added;
}

void AHTTPHeader::parse(const AEmittable& source)
{
  AString strHeader;
  source.emit(strHeader);
  
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
        m_Pairs.insert(MMAP_AString_NVPair::value_type(nvpair.getName(), nvpair));
        AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AHTTPHeader);  //a_Debug only limit
      }
    }
  }
}

bool AHTTPHeader::parseLineZero(const AString &line)
{
  mstr_LineZero = line;
  return _parseLineZero();
}

void AHTTPHeader::parseTokenLine(const AString &line)
{
  AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AHTTPHeader);  //a_Debug only limit
  
  ANameValuePair nvpair(ANameValuePair::HTTP);
  size_t u4Pos = 0;
  nvpair.parse(line, u4Pos);

  if (!nvpair.getName().isEmpty())
  {
    //a_Check to see if the request and response was to handle it
    if (!_handledByChild(nvpair))
    {
      if (m_Pairs.find(nvpair.getName()) != m_Pairs.end())
        ATHROW_EX(this, AException::DataConflict, nvpair.getName());

      //a_Child did not handle it, we will
      m_Pairs.insert(MMAP_AString_NVPair::value_type(nvpair.getName(), nvpair));
    }
  }
}

void AHTTPHeader::set(const AString& name, const AString& value)
{
  remove(name);
  add(name, value);
}

void AHTTPHeader::add(const AString& name, const AString& value)
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AHTTPHeader);
  m_Pairs.insert(MMAP_AString_NVPair::value_type(name, ANameValuePair(name, value, ANameValuePair::HTTP)));
}

void AHTTPHeader::remove(const AString& name)
{
  AASSERT(this, name.findOneOf(AConstant::CHARSET_UPPERCASE_LETTERS));
  MMAP_AString_NVPair::iterator it = m_Pairs.find(name);
  while (it != m_Pairs.end())
  {
    m_Pairs.erase(it);
    it = m_Pairs.find(name);
  }
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
  if (get(AHTTPHeader::HT_GEN_Keep_Alive, str))
    return str.toSize_t();
  else
    return AConstant::npos;
}

const AString& AHTTPHeader::getVersion() const
{ 
  return mstr_HTTPVersion;
}

size_t AHTTPHeader::getContentLength() const
{
  AString str;
  if (get(AHTTPHeader::HT_ENT_Content_Length, str))
    return str.toSize_t();
  else
    return AConstant::npos;
}
