#ifndef INCLUDED__AConstant_HPP__
#define INCLUDED__AConstant_HPP__

#include "apiABase.hpp"

class AString;

class ABASE_API AConstant
{
public:
  static const size_t npos;         //a_Defined as -1
  static const size_t MAX_SIZE_T;   //a_max value of size_t type

  //a_Constants
  static const AString ASTRING_EMPTY;         //a_""  empty string
  static const AString ASTRING_ZERO;          //a_"0" zero
  static const AString ASTRING_ONE;           //a_"1" one
  static const AString ASTRING_TWO;           //a_"2" two
  static const AString ASTRING_TAB;           //a_"\t" one tab
  static const AString ASTRING_SPACE;         //a_" " one space
  static const AString ASTRING_TWOSPACES;     //a_"  " two spaces
  static const AString ASTRING_SINGLEQUOTE;   //a_' single quote
  static const AString ASTRING_DOUBLEQUOTE;   //a_" double quote
  static const AString ASTRING_WHITESPACE;    //a_" \r\n\t" whitespace
  static const AString ASTRING_NULL;          //a_"(null)" something to denote null value
  static const AString ASTRING_COLON;         //a_":" colon
  static const AString ASTRING_SEMICOLON;     //a_";" semicolon
  static const AString ASTRING_PERIOD;        //a_"." period
  static const AString ASTRING_COMMA;         //a_"," comma
  static const AString ASTRING_LESSTHAN;      //a_"<"
  static const AString ASTRING_GREATERTHAN;   //a_">"
  static const AString ASTRING_OPENBRACE;     //a_"{"
  static const AString ASTRING_CLOSEBRACE;    //a_"}"
  static const AString ASTRING_QUESTIONMARK;  //a_"?"
  static const AString ASTRING_UNDEFINED;     //a_"undefined"
  static const AString ASTRING_EQUALS;        //a_"="
  static const AString ASTRING_SLASH;         //a_"/"
  static const AString ASTRING_DOUBLESLASH;    //a_"//"
  static const AString ASTRING_TRUE;          //a_"true"
  static const AString ASTRING_FALSE;         //a_"false"
  static const AString ASTRING_CR;            //a_"\r" always
  static const AString ASTRING_LF;            //a_"\n" always
  static const AString ASTRING_CRLF;          //a_"\r\n" always
  static const AString ASTRING_CRLFCRLF;      //a_"\r\n\r\n" always
  static const AString ASTRING_EOL;           //a_"\r\n" on Windows, "\n" otherwise

  static const AString CHARSET_URLSAFE_ALPHANUM;      //a_"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=_"
  static const AString CHARSET_ALPHANUM;              //a_"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
  static const AString CHARSET_UPPERCASE_LETTERS;     //a_"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  static const AString CHARSET_LOWERCASE_LETTERS;     //a_"abcdefghijklmnopqrstuvwxyz"
  static const AString CHARSET_LOWERCASE_CONSONANTS;  //a_"bcdfghjklmnpqrstvwxyz"
  static const AString CHARSET_LOWERCASE_VOWELS;      //a_"aeiou"
  static const AString CHARSET_NUMBERS;               //a_"0123456789"

  /*!
  Base 10 set
  "0123456789"
  */
  static const AString CHARSET_BASE10;

  /*!
  Base 16 set (hexadecimal)
  "0123456789ABCDEF"
  */
  static const AString CHARSET_BASE16;

  /*!
  Character set to be used by Base64
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   0000000000000000111111111111111122222222222222223333333333333333
   0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
  */
  static const AString CHARSET_BASE64;

  /*!
  Character set to be used by AlphaNum
  "9kB7CtIyE8FwGH-fKbL6MjNOPQRSTarUl3VcWvx5YZ_dD14ghiXm0noDspJquz2e";
   0000000000000000111111111111111122222222222222223333333333333333
   0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
  */
  static const AString CHARSET_ALPHANUM64;

  /*!
  Valid URI characters that do not need to be encoded via URL %XX scheme
  As per RFC-2396
  */
  static const AString CHARSET_URL_RFC2396;
};

#endif //INCLUDED__AConstant_HPP__