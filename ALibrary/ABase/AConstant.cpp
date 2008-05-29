/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AConstant.hpp"
#include <limits.h>
#include "AString.hpp"

#if defined(_WIN64)
const size_t AConstant::MAX_SIZE_T(_UI64_MAX);  // 0xffffffffffffffffui64
#elif defined(_WIN32)
const size_t AConstant::MAX_SIZE_T(_UI32_MAX);  // 0xffffffffui32
#else
#pragma error("Unknown size_t max size")
#endif

const AString AConstant::ASTRING_EMPTY;
const AString AConstant::ASTRING_ZERO("0",1);
const AString AConstant::ASTRING_ONE("1",1);
const AString AConstant::ASTRING_TWO("2",1);
const AString AConstant::ASTRING_SINGLEQUOTE("'",1);
const AString AConstant::ASTRING_DOUBLEQUOTE("\"",1);
const AString AConstant::ASTRING_TAB("\t",1);
const AString AConstant::ASTRING_SPACE(" ",1);
const AString AConstant::ASTRING_TWOSPACES("  ",2);
const AString AConstant::ASTRING_WHITESPACE(" \r\n\t",4);
const AString AConstant::ASTRING_NULL("(null)",6);
const AString AConstant::ASTRING_PERIOD(".",1);
const AString AConstant::ASTRING_DOUBLEPERIOD("..",2);
const AString AConstant::ASTRING_COLON(":",1);
const AString AConstant::ASTRING_SEMICOLON(";",1);
const AString AConstant::ASTRING_COMMA(",",1);
const AString AConstant::ASTRING_LESSTHAN("<",1);
const AString AConstant::ASTRING_GREATERTHAN(">",1);
const AString AConstant::ASTRING_OPENBRACE("{",1);
const AString AConstant::ASTRING_CLOSEBRACE("}",1);
const AString AConstant::ASTRING_QUESTIONMARK("?",1);
const AString AConstant::ASTRING_UNDEFINED("undefined",9);
const AString AConstant::ASTRING_EQUALS("=",1);
const AString AConstant::ASTRING_SLASH("/",1);
const AString AConstant::ASTRING_DOUBLESLASH("//",2);
const AString AConstant::ASTRING_TRUE("true",4);
const AString AConstant::ASTRING_FALSE("false",5);
const AString AConstant::ASTRING_CR("\r",1);
const AString AConstant::ASTRING_LF("\n",1);
const AString AConstant::ASTRING_CRLF("\r\n",2);
const AString AConstant::ASTRING_CRLFCRLF("\r\n\r\n",4);

const AString AConstant::CHARSET_URLSAFE_ALPHANUM("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_$.+",67);
const AString AConstant::CHARSET_ALPHANUM("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",62);
const AString AConstant::CHARSET_UPPERCASE_LETTERS("ABCDEFGHIJKLMNOPQRSTUVWXYZ",26);
const AString AConstant::CHARSET_LOWERCASE_LETTERS("abcdefghijklmnopqrstuvwxyz",26);
const AString AConstant::CHARSET_LOWERCASE_CONSONANTS("bcdfghjklmnpqrstvwxyz",21);
const AString AConstant::CHARSET_LOWERCASE_VOWELS("aeiou",5);
const AString AConstant::CHARSET_NUMBERS("0123456789",10);

const AString AConstant::CHARSET_BASE10("0123456789", 10);
const AString AConstant::CHARSET_BASE16("0123456789ABCDEF", 16);

//a_Character map used for Base64 conversions
const AString AConstant::CHARSET_BASE64 = 
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
// 0000000000000000111111111111111122222222222222223333333333333333
// 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF

//a_Character map used for Alphanum conversions
const AString AConstant::CHARSET_ALPHANUM64 = 
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
// 0000000000000000111111111111111122222222222222223333333333333333
// 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF

const AString AConstant::CHARSET_URL_RFC2396 = 
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789;/?:@&=+$,-_.!~*'()";
// 000000000000000011111111111111112222222222222222333333333333333344444444444444445
// 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0

#if defined(__WINDOWS__)
const AString AConstant::ASTRING_EOL("\r\n");
#else
const AString AConstant::ASTRING_EOL("\n");
#endif
