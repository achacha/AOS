/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AHTTPHeader_HPP__
#define INCLUDED__AHTTPHeader_HPP__

#include "apiABase.hpp"
#include "ANameValuePair.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "AParsable.hpp"

class ABASE_API AHTTPHeader : public ADebugDumpable, public ASerializable, public AXmlEmittable, public AParsable
{
public:
  /*!
  Tokens from RFC-2068
  */
  enum HEADER_TOKENS
  {
    HT_INVALID                  = 0x00000000,
    
    //! General-header
    HT_GEN_Cache_Control        = 0x10000001,
    HT_GEN_Connection           = 0x10000002,
    HT_GEN_Date                 = 0x10000003,
    HT_GEN_Pragma               = 0x10000004,
    HT_GEN_Transfer_Encoding    = 0x10000005,
    HT_GEN_Upgrade              = 0x10000006,
    HT_GEN_Via                  = 0x10000007,
    HT_GEN_Keep_Alive           = 0x10000008,

    //! Request-header
    HT_REQ_Accept               = 0x10000020,
    HT_REQ_Accept_Charset       = 0x10000021,
    HT_REQ_Accept_Encoding      = 0x10000022,
    HT_REQ_Accept_Language      = 0x10000023,
    HT_REQ_Accept_Ranges        = 0x10000024,
    HT_REQ_Authorization        = 0x10000025,
    HT_REQ_From                 = 0x10000026,
    HT_REQ_Host                 = 0x10000027,
    HT_REQ_If_Modified_Since    = 0x10000028,
    HT_REQ_If_Match             = 0x10000029,
    HT_REQ_If_None_Match        = 0x1000002A,
    HT_REQ_If_Range             = 0x1000002B,
    HT_REQ_If_Unmodified_Since  = 0x1000002C,
    HT_REQ_Max_Forwards         = 0x1000002D,
    HT_REQ_Proxy_Authorization  = 0x1000002E,
    HT_REQ_Range                = 0x1000002F,
    HT_REQ_Referer              = 0x10000030,
    HT_REQ_User_Agent           = 0x10000031,
    
    //! Special request-header
    HT_REQ_Cookie               = 0x1000003A,

    //! Response-header
    HT_RES_Age                  = 0x10000040,
    HT_RES_Location             = 0x10000041,
    HT_RES_Proxy_Authenticate   = 0x10000042,
    HT_RES_Public               = 0x10000043,
    HT_RES_Retry_After          = 0x10000044,
    HT_RES_Server               = 0x10000045,
    HT_RES_TE                   = 0x10000046,      
    HT_RES_Vary                 = 0x10000047,
    HT_RES_Warning              = 0x10000048,
    HT_RES_WWW_Authenticate     = 0x10000049,

    //a_Special response-header
    HT_RES_Set_Cookie           = 0x1000004A,

    //a_Entity-header
    HT_ENT_Allow                = 0x10000060,
    HT_ENT_Content_Base         = 0x10000061,
    HT_ENT_Content_Encoding     = 0x10000062,
    HT_ENT_Content_Language     = 0x10000063,
    HT_ENT_Content_Length       = 0x10000064,
    HT_ENT_Content_Location     = 0x10000065,
    HT_ENT_Content_MD5          = 0x10000066,
    HT_ENT_Content_Range        = 0x10000067,
    HT_ENT_Content_Type         = 0x10000068,
    HT_ENT_ETag                 = 0x10000069,
    HT_ENT_Expires              = 0x1000006A,
    HT_ENT_Last_Modified        = 0x1000006B
  };

  //! HTML POST form: application/x-www-form-urlencoded
  static const AString CONTENT_TYPE_HTML_FORM;

  //! HTML multi-part POST form: multipart/form-data
  static const AString CONTENT_TYPE_HTML_FORM_MULTIPART;

  //! String that denotes default HTTP version supported "HTTP/1.1"
  static const AString DEFAULT_HTTP_VERSION;

public:
  virtual ~AHTTPHeader();

  //a_Presentation
  virtual void emit(AOutputBuffer&) const;
  
  //! Clear
  virtual void clear();
  
  //! Parse from AEmittable
  virtual void parse(const AEmittable&);

  /*!
  ASerializable (writing to AFile type)
  */
  virtual void toAFile(AFile&) const;

  /*!
  ASerializable (reading from AFile type)
  */
  virtual void fromAFile(AFile&);

  /*!
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
  
  @param line0 to parse
  */
  bool parseLineZero(const AString & line0);
  
  /*!
  Parse token lines after line 0
  These contain the name: value pairs

  @param line to parse
  */
  void parseTokenLine(const AString & line);

  /*!
  Number of header pairs

  @return number of pairs in the header
  */
  size_t size() const;
  
  /*!
  Set header pair

  @param eToken name from HEADER_TOKEN enum
  @param value of the header pair
  @see AHTTPHeader::HEADER_TOKEN
  */
  void setPair(AHTTPHeader::HEADER_TOKENS eToken, const AString& value);

  /*!
  Set header pair

  @param name of the header pair
  @param value of the header pair
  */
  void setPair(const AString& name, const AString& value);
  
  /*!
  Remove header pair

  @param eToken name from HEADER_TOKEN enum
  @see AHTTPHeader::HEADER_TOKEN
  */
  void removePair(AHTTPHeader::HEADER_TOKENS eToken);

  /*!
  Checks existance of a header pair

  @param eToken name from HEADER_TOKEN enum
  @see AHTTPHeader::HEADER_TOKEN
  */
  bool exists(AHTTPHeader::HEADER_TOKENS eToken) const;
  
  /*!
  Compares value to the string, returns false if not equals or not found

  @param eToken name from HEADER_TOKEN enum
  @param value to compare to
  @see AHTTPHeader::HEADER_TOKEN
  */
  bool equals(AHTTPHeader::HEADER_TOKENS eToken, const AString& value) const;
  
  /*!
  Compares value to the string ignoring case, returns false if not equals or not found

  @param eToken name from HEADER_TOKEN enum
  @param value to compare to without case
  @see AHTTPHeader::HEADER_TOKEN
  */
  bool equalsNoCase(AHTTPHeader::HEADER_TOKENS eToken, const AString& value) const;

  /*!
  Find a token and append the value to target

  @param eToken name from HEADER_TOKEN enum
  @param target to emit to
  @see AHTTPHeader::HEADER_TOKEN
  */
  bool getPairValue(AHTTPHeader::HEADER_TOKENS eToken, AOutputBuffer& target) const;
  
  /*!
  Find a token as a string and append to target
  
  @param name of the header pair
  @param target to emit to
  */
  bool getPairValue(const AString& name, AOutputBuffer& target) const;

  /*!
  Version of the header (supported by both response and request)
  
  @return HTTP version string
  */
  const AString& getVersion() const;
  
  /*!
  Set version
  Valid versions:  "HTTP/0.9"  "HTTP/1.0"  "HTTP/1.1"
  Invalid version will throw exception

  @param version to set to, must be valid
  */
  void setVersion(const AString& version);

  /*!
  Utility functions used by both header types
  Checks if HTTP/0.9 or HTTP/1.0 or HTTP/1.1
  
  @return true if version string is valid
  */
  bool isValidVersion();

  /*!
  Keep-Alive timeout value or AConstant::npos if none
  
  @return timeout value from keep-alive: as size_t
  */
  size_t getKeepAliveTimeout() const;

  /*!
  Helper to get Content-Length

  @return content length as size_t
  */
  size_t getContentLength() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //! default ctor without any cookie processing
  AHTTPHeader();
  
  //! copy
  AHTTPHeader(const AHTTPHeader&);
  AHTTPHeader& operator=(const AHTTPHeader&);

  //! Storage of name value pairs
  MAP_AString_NVPair m_Pairs;
  
  //! Managed by the request and response derived classes
  AString mstr_LineZero;
  AString mstr_HTTPVersion;

  //! First line is always a special case parsed by a child
  virtual bool _parseLineZero() = 0;  
  
  //! If this pair is handled by child it will return 1, else 0 means no and added to this object
  virtual bool _handledByChild(const ANameValuePair& nvPair) = 0;  

  // Mapping from types to strings
  const AString              _mapTypeToString(AHTTPHeader::HEADER_TOKENS eToken) const;
  AHTTPHeader::HEADER_TOKENS _mapStringToType(const AString& strToken) const;

  // Status code description
  AString _getStatusCodeDescription(int iStatusCode);

  void _copy(const AHTTPHeader&);
};

#endif

