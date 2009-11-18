/*
Written by Alex Chachanashvili

$Id$
*/
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
  static const AString CDATA_UNSAFE;
  static const AString CDATA_SAFE;
  static const AString HTML_UNSAFE;
  static const AString HEXDUMP_PAD;
  static const AString FILENAME_SAFE;

public:
  /*!
  Encoding Base64 (ratio input:output=3:4)
  Faster than generic encode64, optimized for Base64

  @param source string to encode
  @param target to append encoded string to
  */
  static void encodeBase64(const AString& source, AOutputBuffer& target);

  /*!
  Decoding Base64 (ratio input:output=4:3)
  Faster than generic decode64, optimized for Base64

  @param source string to decode
  @param target to append decoded string to
  */
  static void decodeBase64(const AString& source, AOutputBuffer& target);

  /*!
  Decoding Base64 at a given position in the source (ratio input:output=4:3)
  Faster than generic decode64, optimized for Base64
  Allows a starting position (for encoded data embedded inside a string, as in HTTP headers)

  @param source string to decode
  @param startPos start position inside the source
  @param target to append decoded string to
  */
  static void decodeBase64(const AString& source, size_t startPos, AOutputBuffer& target);

  /*!
  Encoding arbitrary 64 character set (ratio input:output=3:4)
  
  @param source to encode
  @param target to append result to
  @param charset to use as lookup, MUST be 64 characters in size, default is database/URL friendly AlphaNum set
  */
  static void encode64(const AString& source, AOutputBuffer& target, const AString& charset = AConstant::CHARSET_ALPHANUM64);

  /*!
  Decoding arbitrary 64 character set (ratio input:output=4:4)

  @param source to decode
  @param target to append result to
  @param charset to use as lookup, MUST be 64 characters in size, default is database/URL friendly AlphaNum set
  */
  static void decode64(const AString& source, AOutputBuffer& target, const AString& charset = AConstant::CHARSET_ALPHANUM64);

  /*!
  URL encoding
  Alphanum is passthru, ' ' to '+', and rest to %XX
  If encoding a URL, set isUrlPerRFC2396 to true and it will skip over the characters that are part of the URL (such as : / ~ @ etc) but will encode everything else as per RFC-2396 spec

  @param source to encode
  @param target to append result to
  @param isUrlPerRFC2396 - if true, assumes that an entire URL is being encoded and will not encode characteres that are special (used to keep URLs as URLs while encoding anythign else that is not valid when parsing URLs)
  */
  static void encodeURL(const AString& source, AOutputBuffer& target, bool isUrlPerRFC2396 = false);

  /*!
  URL encoding/decoding
  Alphanum is passthru, ' ' to '+' and rest to %XX

  @param source to decode
  @param target to append result to
  @param ignoreNulls if to ignore null after decoding
  @param forceUppercase to convert alpha to uppercase after decoding
  */
  static void decodeURL(const AString& source, AOutputBuffer& target, bool ignoreNulls = true, bool forceUppercase = false);

  /*!
  Public routines for encoding and decoding HEX dumps (0xNN becomes "NN")
  Binary string to 2 digit hex ASCII value
  
  @param source to encode
  @param target to append result to
  */
  static void encodeHEX(const AString& source, AOutputBuffer& target);  

  /*!
  Public routines for encoding and decoding HEX dumps (0xNN becomes "NN")
  Binary string to 2 digit hex ASCII value

  @param source to decode
  @param target to append result to
  */
  static void decodeHEX(const AString& source, AOutputBuffer& target);  

  /*!
  HEX to BYTE: Converts high byte 'A' and low byte 'B' to 8-bit hex value of 0xAB (decimal 171)

  @param cHi high byte
  @param cLo low byte
  @return 1 byte (8-bit) value
  */
  static u1 convertHEXtoBYTE(char cHi, char cLo);

  /*!
  BYTE to HEX: Converts 8-bit value 0xAB (decimal 171) to string "AB"

  @param byteV 8-bit value to convert to hex string representation
  @return Always 2 character string representing the value (leads with 0 if byteV < 16)
  */
  static AString convertBYTEtoHEX(u1 byteV);

  /*!
  BYTE to HEX: Converts 8-bit value 0xAB (decimal 171) to string "AB", appends result

  @param byteV 8-bit value to convert to hex string representation
  @param target to append result to, always 2 characters appended (adds leading 0 if byteV < 16)
  */
  static void convertBYTEtoHEX(u1 byteV, AOutputBuffer& target);

  /*!
  Make HTML safe (converts < to &lt; > to &gt; and & to &amp;)

  @param source to convert
  @param target to append result to
  */
  static void makeHtmlSafe(const AString& source, AOutputBuffer& target);   

  /*!
  Make CData safe  (replace ]]> with %5d%5d%3e)

  @param source to convert
  @param target to append result to
  */
  static void makeCDataSafe(const AString& source, AOutputBuffer& target);  

  /*!
  Coverts to ASCII printable string with non-alphanum encoded in C style escaping (\\xNN)
  Useful for including messages with user input into XML or screen dump

  @param source to convert
  @param target to append result to
  */
  static void makeAsciiPrintable(const AString& source, AOutputBuffer& target);

  /*!
  Make the string somewhat database safe by removing some special characters
  Helps prevent SQL injection
  Currently replaes ' with ` characters
  It is safer to use encodeURL and write that into the database and decode on the way out

  @param source to convert
  @param target to append result to
  */
  static void makeSQLSafe(const AString& source, AOutputBuffer& target);

  /*!
  Make the string safe as a filename
  Any unsafe characters will be converted to _

  @param source to make safe
  @param target to append result to
  */
  static void makeFilenameSafe(const AString& source, AOutputBuffer& target);

  /*!
  String conversion to hex dump (ratio input:output=1:2)
  By default converts non-ASCII chacaters to '.'

  @param source to convert
  @param target to append result to
  @param includeNonAscii if true will not convert non ASCII characters to '.' but keep them in the ASCII portion of the dump
  */
  static void convertStringToHexDump(const AString& source, AOutputBuffer& target, bool includeNonAscii = false);    

private:
  //a_Decode64 helper
  static int _decodeBase64Character(int iCharacter);
};

#endif

