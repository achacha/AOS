#ifndef INCLUDED__AHTTPHeader_HPP__
#define INCLUDED__AHTTPHeader_HPP__

#include "apiABase.hpp"
#include "ANameValuePair.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"

class ABASE_API AHTTPHeader : public ADebugDumpable, public ASerializable, public AXmlEmittable
{
public:
  //a_Tokens from RFC-2068
  enum HEADER_TOKENS
  {
    HT_INVALID                  = 0x0000000,
    
    //a_General-header
    HT_GEN_Cache_Control        = 0x10000001,
    HT_GEN_Connection           = 0x10000002,
    HT_GEN_Date                 = 0x10000003,
    HT_GEN_Pragma               = 0x10000004,
    HT_GEN_Transfer_Encoding    = 0x10000005,
    HT_GEN_Upgrade              = 0x10000006,
    HT_GEN_Via                  = 0x10000007,
    HT_GEN_Keep_Alive           = 0x10000008,

    //a_Request-header
    HT_REQ_Accept               = 0x10000010,
    HT_REQ_Accept_Charset       = 0x10000011,
    HT_REQ_Accept_Encoding      = 0x10000012,
    HT_REQ_Accept_Language      = 0x10000013,
    HT_REQ_Accept_Ranges        = 0x10000014,
    HT_REQ_Authorization        = 0x10000015,
    HT_REQ_From                 = 0x10000016,
    HT_REQ_Host                 = 0x10000017,
    HT_REQ_If_Modified_Since    = 0x10000018,
    HT_REQ_If_Match             = 0x10000019,
    HT_REQ_If_None_Match        = 0x1000001A,
    HT_REQ_If_Range             = 0x1000001B,
    HT_REQ_If_Unmodified_Since  = 0x1000001C,
    HT_REQ_Max_Forwards         = 0x1000001D,
    HT_REQ_Proxy_Authorization  = 0x1000001E,
    HT_REQ_Range                = 0x1000001F,
    HT_REQ_Referer              = 0x10000020,
    HT_REQ_User_Agent           = 0x10000021,
    HT_RES_TE                   = 0x10000022,      
                                            
    //a_Response-header
    HT_RES_Age                  = 0x10000030,
    HT_RES_Location             = 0x10000031,
    HT_RES_Proxy_Authenticate   = 0x10000032,
    HT_RES_Public               = 0x10000033,
    HT_RES_Retry_After          = 0x10000034,
    HT_RES_Server               = 0x10000035,
    HT_RES_Vary                 = 0x10000036,
    HT_RES_Warning              = 0x10000037,
    HT_RES_WWW_Authenticate     = 0x10000038,

    //a_Entity-header
    HT_ENT_Allow                = 0x10000050,
    HT_ENT_Content_Base         = 0x10000051,
    HT_ENT_Content_Encoding     = 0x10000052,
    HT_ENT_Content_Language     = 0x10000053,
    HT_ENT_Content_Length       = 0x10000054,
    HT_ENT_Content_Location     = 0x10000055,
    HT_ENT_Content_MD5          = 0x10000056,
    HT_ENT_Content_Range        = 0x10000057,
    HT_ENT_Content_Type         = 0x10000058,
    HT_ENT_ETag                 = 0x10000059,
    HT_ENT_Expires              = 0x10000060,
    HT_ENT_Last_Modified        = 0x10000061
  };

public:
  virtual ~AHTTPHeader();

  //a_Presentation
  virtual void emit(AOutputBuffer&) const;
  
  //a_Parse and clear
  virtual void clear();
  virtual void parse(const AString& strHeader);

  /*
  ASerializable (reading and writing to AFile type)
  */
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);

  /*
    Used in quick parsing of headers
    ...
    file.readLine(str);
    if (parseFirstLine(str))
    {
      while (file.readLine(str))
        parseToeknLine(str);
    }
    else
      //display some error about error in line 0
    ...
  */
  bool parseLineZero(const AString &);
  void parseTokenLine(const AString &);

  //a_Access
  size_t size() const { return m_Pairs.size(); }
  void setPair(AHTTPHeader::HEADER_TOKENS eToken, const AString& strValue);
  void setPair(const AString& strName, const AString& strValue);
  void removePair(AHTTPHeader::HEADER_TOKENS eToken);

  //a_Checks existance
  bool exists(AHTTPHeader::HEADER_TOKENS eToken) const;
  
  //a_Compares value to the string, returns false if not equals or not found
  bool equals(AHTTPHeader::HEADER_TOKENS eToken, const AString&) const;
  
  /*!
  Find a token and assigns to strDest and returns true,
    else returns false without changing the strDest
  */
  bool find(AHTTPHeader::HEADER_TOKENS eToken, AString& strDest) const;
  bool find(const AString& strTokenName, AString& strDest) const;

  /*!
  Version of the header (supported by both response and request)
  */
  const AString& getVersion() const;
  
  /*!
  Set version
  Valid versions:  "HTTP/0.9"  "HTTP/1.0"  "HTTP/1.1"
  Invalid version will throw exception
  */
  void setVersion(const AString&);

  /*!
  Utility functions used by both header types
  Checks if HTTP/0.9 or HTTP/1.0 or HTTP/1.1
  */
  bool isValidVersion();

  /*!
  Keep-Alive timeout value or AConstant::npos if none
  */
  size_t AHTTPHeader::getKeepAliveTimeout() const;

  //a_String that denotes default HTTP version supported "HTTP/1.1"
  static const AString DEFAULT_HTTP_VERSION;

protected:
  //a_This object does not exist by itself, but it's childer will exist on their own
  AHTTPHeader();
  AHTTPHeader(const AHTTPHeader&);
  AHTTPHeader& operator=(const AHTTPHeader&);

  //a_Storage of name value pairs
  MAP_AString_NVPair m_Pairs;
  
  //a_Managed by the request and response derived classes
  AString mstr_LineZero;
  AString mstr_HTTPVersion;

  //a_First line is always a special case parsed by a child
  virtual bool _parseLineZero() = 0;  
  
  //a_if this pair is handled by child it will return 1, else 0 means no and added to this object
  virtual bool _handledByChild(const ANameValuePair& nvPair) = 0;  

  //a_Mapping from types to strings
  const AString              _mapTypeToString(AHTTPHeader::HEADER_TOKENS eToken) const;
  AHTTPHeader::HEADER_TOKENS _mapStringToType(const AString& strToken) const;

  //a_Status code description
  AString _getStatusCodeDescription(int iStatusCode);

  void _copy(const AHTTPHeader&);

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif

