/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef __ANameValuePair_HPP__
#define __ANameValuePair_HPP__

#include "apiABase.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"

class ABASE_API ANameValuePair;

//a_Containers involving ANameValuePair
typedef std::list<ANameValuePair>                    LIST_NVPair;
typedef std::list<ANameValuePair *>                  LIST_NVPairPtr;
typedef std::vector<ANameValuePair>                  VECTOR_NVPair;
typedef std::vector<ANameValuePair *>                VECTOR_NVPairPtr;
typedef std::map<const AString, ANameValuePair>      MAP_AString_NVPair;
typedef std::map<const AString, ANameValuePair>      MAP_AString_NVPairPtr;
typedef std::multimap<const AString, ANameValuePair> MMAP_AString_NVPair;
typedef std::multimap<const AString, ANameValuePair> MMAP_AString_NVPairPtr;

class ABASE_API ANameValuePair : virtual public ASerializable, virtual public ADebugDumpable
{
public:
  //a_ "=" and " " or "/>"  or ">" for HTML/XML attributes (special case)
  //a_ "=" and "&" for CGI and CGI_CASE parameters (CGI_CASE is case sensitive name)
  //a_ ": " and "" for HTTP header parameters
  //a_ "=" amd "" for REGINI with name and value enclosed in quotes (used in WinNT .reg files)
  //a_ "=" and ";" for COOKIE and DATA parameters
  //a_ "=" and ";" for FORM_MULTIPART and value may be enclosed in quotes
  //a_ ":" and "}" for JSON
  enum NameValueType { 
    HTML, 
    XML, 
    CGI, 
    HTTP, 
    COOKIE, 
    REGINI, 
    CGI_CASE, 
    JSON, 
    DATA,
    FORM_MULTIPART,
    CUSTOM=99 
  };

public:
  //a_ctor/dtor/etc
  //a_Parses the input (default to CGI type)
  ANameValuePair(const AString& strName, const AString& strValue = AConstant::ASTRING_EMPTY, NameValueType eType = ANameValuePair::CGI);
  ANameValuePair(NameValueType eType = ANameValuePair::CGI);
  ~ANameValuePair() {}     //a_No virtual methods, no virtula destructor

  //a_Parsing method
  void parse(const AString& strInput, size_t& pos);

  //a_Access methods
  inline const AString& getName()  const;
  inline const AString& getValue() const;

  //a_Comparisson
  int compare(const ANameValuePair&) const;
  
  //a_Case-sensitive string compare to name
  bool isName(const AString&) const;

  //a_Case-insensitive string compare to name
  bool isNameNoCase(const AString&) const;

  //a_Case-sensitive string compare to value
  bool isValue(const AString&) const;

  /*!
    Set the type of the pair
    The terminator, separator, value wrap and whitespace are automatically set for the new type
  */
  void setType(NameValueType eType);

  /*!
    Set custom separator/terminator
    strExactSeparator is the string that must match EXACTLY to denote the sart of VALUE
    strOneOfTerminators is a list of characters that can mean an end of the NAME/VALUE pair
  */
  void setDelimeters(
    const AString &strExactSeparator = AConstant::ASTRING_EMPTY, 
    const AString &strOneOfTerminators = ASW("&\r\n",3)
  );

  /*!
    Custom value wrap character
      - character to wrap the value (used for parsing and output), HTML/XML uses \", rest are \x0
  */
  void setValueWrap(char);

  /*!
    Custom whitespace
  */
  void setWhiteSpace(const AString &);

  //a_Assumes name/value pair is valid
  void setName(const AString& strName);
  void setNameValue(const AString& strName, const AString& strValue);
  void setValue(const AString& strValue);

  //a_Represent as a string of form 'name=value'
  virtual void emit(AOutputBuffer&) const;
  
  //a_Clear the pair
  void clear();
  
	/*!
  ASerializable
  */
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Utility functions
  inline bool __isWhiteSpace(char cX);

  //a_Parsing methods
  void _parseHtmlValue(const AString &strInput, size_t& pos, AString &strReturn);
  void _parseREGINIValue(const AString &strInput, size_t& pos, AString &strReturn);
  void _parseJSONValue(const AString &strInput, size_t& pos, AString &strReturn);
  void _parseValue(const AString &strInput, size_t& pos, AString &strReturn);
  void _parseREGININame(const AString &strInput, size_t& pos, AString &strReturn);
  void _parseName(const AString &strInput, size_t& pos, AString &strReturn);

  //a_The NAME/VALUE data
  AString m_name;
  AString m_value;

  //a_Delimeters
  NameValueType m_nvtype;
  AString m_separator;       //a_Separates NAME and VALUE
  AString m_terminator;      //a_Delimits start of next pair or <EOS>
  AString m_whitespace;      //a_Characters to treat as white space (ie. ignore)
  char m_valueWrap;          //a_Normally \x0 to mean none, but can be either ' or ", can change during parsing
};

#endif

