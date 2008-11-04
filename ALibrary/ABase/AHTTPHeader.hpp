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
  Field names from RFC-2616 (HTTP/1.1)

  By definition field-names are case insensitive and stored as all lowercase
  */
  static const AString HT_GEN_Cache_Control;
  static const AString HT_GEN_Connection;
  static const AString HT_GEN_Date;
  static const AString HT_GEN_Pragma;
  static const AString HT_GEN_Transfer_Encoding;
  static const AString HT_GEN_Upgrade;
  static const AString HT_GEN_Via;
  static const AString HT_GEN_Keep_Alive;
  static const AString HT_REQ_Accept;
  static const AString HT_REQ_Accept_Charset;
  static const AString HT_REQ_Accept_Encoding;
  static const AString HT_REQ_Accept_Language;
  static const AString HT_REQ_Accept_Ranges;
  static const AString HT_REQ_Authorization;
  static const AString HT_REQ_From;
  static const AString HT_REQ_Host;
  static const AString HT_REQ_If_Modified_Since;
  static const AString HT_REQ_If_Match;
  static const AString HT_REQ_If_None_Match;
  static const AString HT_REQ_If_Range;
  static const AString HT_REQ_If_Unmodified_Since;
  static const AString HT_REQ_Max_Forwards;
  static const AString HT_REQ_Proxy_Authorization;
  static const AString HT_REQ_Range;
  static const AString HT_REQ_Referer;
  static const AString HT_REQ_User_Agent;
  static const AString HT_REQ_Cookie;
  static const AString HT_RES_Age;
  static const AString HT_RES_Location;
  static const AString HT_RES_Proxy_Authenticate;
  static const AString HT_RES_Public;
  static const AString HT_RES_Retry_After;
  static const AString HT_RES_Server;
  static const AString HT_RES_Vary;
  static const AString HT_RES_Warning;
  static const AString HT_RES_WWW_Authenticate;
  static const AString HT_RES_TE;
  static const AString HT_RES_Set_Cookie;
  static const AString HT_ENT_Allow;
  static const AString HT_ENT_Content_Base;
  static const AString HT_ENT_Content_Encoding;
  static const AString HT_ENT_Content_Language;
  static const AString HT_ENT_Content_Length;
  static const AString HT_ENT_Content_Location;
  static const AString HT_ENT_Content_MD5;
  static const AString HT_ENT_Content_Range;
  static const AString HT_ENT_Content_Type;
  static const AString HT_ENT_ETag;
  static const AString HT_ENT_Expires;
  static const AString HT_ENT_Last_Modified;

  //! HTML POST form: application/x-www-form-urlencoded
  static const AString CONTENT_TYPE_HTML_FORM;

  //! HTML multi-part POST form: multipart/form-data
  static const AString CONTENT_TYPE_HTML_FORM_MULTIPART;

  //! HTTP/0.9 version string
  static const AString HTTP_VERSION_0_9;
  //! HTTP/1.0 version string
  static const AString HTTP_VERSION_1_0;
  //! HTTP/1.1 version string
  static const AString HTTP_VERSION_1_1;

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
  @return true if parsed successfully
  */
  bool parseLineZero(const AString& line0);
  
  /*!
  Parse token lines after line 0
  These contain the name: value pairs

  @param line to parse
  */
  void parseTokenLine(const AString& line);
  
  /*!
  Access to the container of HTTP name -> value(s)

  @return reference to the container
  */
  MMAP_AString_NVPair& useContainer();

  /*!
  Access to the container of HTTP name -> value(s)

  @return constant reference to the container
  */
  const MMAP_AString_NVPair& getContainer() const;

  /*!
  Add header pair

  @param name of the header pair
  @param value of the header pair
  */
  void add(const AString& name, const AString& value);

  /*!
  Set header pair (replaces the value of the first if already exists and removes the rest)

  @param name of the header pair
  @param value of the header pair
  */
  void set(const AString& name, const AString& value);
  
  /*!
  Remove header pair

  @param name of the field
  */
  void remove(const AString& name);

  /*!
  Checks existance of a header pair

  @param name of the field
  @return true if exists (one or more)
  */
  bool exists(const AString& name) const;
  
  /*!
  Count existance of a header pair

  @param name of the field
  @return number of field name values that exist
  */
  size_t count(const AString& name) const;

  /*!
  Compares value to the string, returns false if not equals or not found
  Will check all field names in the header

  @param name of the field
  @param value to compare to
  */
  bool equals(const AString& name, const AString& value) const;
  
  /*!
  Compares value to the string ignoring case, returns false if not equals or not found
  Will check all field names in the header

  @param name of the field
  @param value to compare to without case
  */
  bool equalsNoCase(const AString& name, const AString& value) const;

  /*!
  Find the field names and appends to target (comma separated)
  
  @param name of the header pair
  @param target to emit to
  @return field values added
  */
  size_t get(const AString& name, AOutputBuffer& target) const;

  /*!
  Find the field names and add to target list
  
  @param name of the header pair
  @param target list to add to
  @return field values added to target
  */
  size_t get(const AString& name, LIST_AString& target) const;

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

  //! Storage of name -> value(s)
  MMAP_AString_NVPair m_Pairs;
  
  //! Managed by the request and response derived classes
  AString mstr_LineZero;
  AString mstr_HTTPVersion;

  //! First line is always a special case parsed by a child
  virtual bool _parseLineZero() = 0;  
  
  //! If this pair is handled by child it will return 1, else 0 means no and added to this object
  virtual bool _handledByChild(const ANameValuePair& nvPair) = 0;  

  // Status code description
  AString _getStatusCodeDescription(int iStatusCode);

  void _copy(const AHTTPHeader&);
};

#endif

