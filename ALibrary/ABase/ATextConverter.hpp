#ifndef INCLUDED__ATextConverter_HPP__
#define INCLUDED__ATextConverter_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "AConstant.hpp"

/*!
Utility class for text conversion and processing

All calls involving AOutputBuffer are append based
*/
class ABASE_API ATextConverter
{
public:
  /*!
  Encoding and decoding Base64 (ratio input:output=3:4)
  Faster than generic encode64/decode64, optimized for Base64
  */
  static void encodeBase64(const AString& strSource, AOutputBuffer&);
  static void decodeBase64(const AString& strSource, AOutputBuffer&);
  static void decodeBase64(const AString& strSource, size_t startPos, AOutputBuffer&);

  /*!
  Encoding and decoding arbitrary 64 character set (ratio input:output=3:4)
  strCharSet MUST be 64 characters in size, default is database/URL friendly AlphaNum set
  */
  static void encode64(const AString& strSource, AOutputBuffer&, const AString& strCharSet = AConstant::CHARSET_ALPHANUM64);
  static void decode64(const AString& strSource, AOutputBuffer&, const AString& strCharSet = AConstant::CHARSET_ALPHANUM64);

  /*!
  URL encoding/decoding
  Alphanum is passthru, ' ' to '+' and rest to %XX
  boolIsValue - when true it will encode 

  isUrlPerRFC2396 - if true will not encode
  */
  static void encodeURL(const AString& strSource, AOutputBuffer&, bool isUrlPerRFC2396 = false);
  static void decodeURL(const AString& strSource, AOutputBuffer&, bool boolIgnoreNulls = true, bool boolForceUppercase = false);

  /*!
  Public routines for encoding and decoding HEX dumps (0xNN becomes "NN")
  Binary string to 2 digit hex ASCII value
  */
  static void encodeHEX(const AString& strSource, AOutputBuffer&);  
  static void decodeHEX(const AString& strSource, AOutputBuffer&);  

  /*!
  Conversion HEX <-> BYTE
  */
  static u1      convertHEXtoBYTE(char cHi, char cLo);
  static AString convertBYTEtoHEX(u1 byteV);
  static void    convertBYTEtoHEX(u1 byteV, AOutputBuffer&);     

  /*!
  HTML safe  (converts < to &lt; > to &gt; and & to &amp;)
  */
  static void makeHtmlSafe(const AString& strSource, AOutputBuffer&);   

  /*!
  CData safe  (replace ]]> with %5d%5d%3e)
  */
  static void makeCDataSafe(const AString& strSource, AOutputBuffer&);  

  /*!
  Coverts to ASCII printable string with non-alphanum encoded in C style escape (\xNN)
  Useful for including messages with user input into XML or screen dump
  */
  static void makeAsciiPrintable(const AString& strSource, AOutputBuffer&);

  /*!
  String conversion to hex dump (ratio input:output=1:2)
  */
  static void convertStringToHexDump(const AString& source, AOutputBuffer&, bool boolIncludeNonAscii = false);    

private:
  //a_Decode64 helper
  static int _decodeBase64Character(int iCharacter);
};

#endif

