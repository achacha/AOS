#ifndef INCLUDED__ATextGenerator_HPP__
#define INCLUDED__ATextGenerator_HPP__

#include "apiABase.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AString.hpp"

class ABASE_API ATextGenerator
{
public:
  /*!
  Generate random numerals
  */
  static void generateRandomNumber(AOutputBuffer& target, size_t len = 1);     //a_Uses numerals only
  
  /*!
  Generate random alphanum
  */
  static void generateRandomAlphanum(AOutputBuffer& target, size_t len = 1);   //a_Uses Upper and lower letters and numerals
  
  /*!
  Generate from template

  Template characters
   %n - number 0-9
   %N - random sign '+' or '-' with 50% probability for each
   %l - lowercase letter a-z
   %L - Uppercase letter A-Z
   %s - Upper or lowercase (50% chance of either)
   %z - Some sequential counter starting at zero at some time
   %t - Current tick count (seconds since Jan 01, 1970)
   %T - Current time in RFC1036 format DD-MMM-YYYY GMT+-TZ etc...
   %r - Random 1-255 character
   %R - Random 0-255 character (includes 0 as a valid character, supported by AString, but not usable as a printable string)
   %% - % symbol
   %<Number> - Ignored, this is URL encoded sequence, where %[0-9,a-z,A-Z][[0-9,a-z,A-Z] is reserved by URL encoding

  Example: SSN: "%n%n%n-%n%n%n-%n%n%n%n"         ->  000-000-0000
           Phone Number: "(212) %n%n%n-%n%n%n%n" ->  (212) 000-0000
           Email: "%s%l%l%l%n%n%n@domain.com"   ->  Aaaa000@domain.com
           Percentage: "Score=%7E%n%n%%"        ->  Score=~66% (After URL decoding, %7E66% before)
  */
  static void generateFromTemplate(AOutputBuffer& target, const AString &strTemplate);
  
  /*!
  Reset the counter used in templates
  Not thread safe, use synch if calling this from more than 1 thread
  */
  static void resetCounter(int iNew = 0x0);

  /*!
  Quasi-random word generator
  */
  static void generateRandomWord(AOutputBuffer& target, size_t len = 1);

  /*!
  Random string based on a charset
  */
  static void generateRandomString(AOutputBuffer& target, size_t len, const AString& strCharSet = AConstant::CHARSET_URLSAFE_ALPHANUM);

  /*!
  Random string for [0,0xFF)  (everything including 0)
  */
  static void generateRandomData(AOutputBuffer& target, size_t len);

  /*!
  Unique identifier based on current time and random number
  Not guaranteed to be unique across processes (but extremely likely to be)
  For guaranteed results use machine specific GUID generator functions
  maxSize must be >16 characters long
  Appends to target up to maxSize
  */
  static void generateUniqueId(AOutputBuffer& target, size_t maxSize = 32);

  /*!
  Some useful helper functions
  */
  static inline char generateRandomLowercaseLetter();   //a_[a,z]
  static inline char generateRandomUppercaseLetter();   //a_[A,Z]
  static inline char generateRandomNumeral();           //a_[0,9]

private:
  //a_Count used by %z parameter
  static int si_Counter;
};

//a_Inlines
inline char ATextGenerator::generateRandomLowercaseLetter()
{
  return ARandomNumberGenerator::get().nextRange(char('z') + 1, char('a'));
}
inline char ATextGenerator::generateRandomUppercaseLetter()
{
  return ARandomNumberGenerator::get().nextRange(char('Z') + 1, char('A'));
}
inline char ATextGenerator::generateRandomNumeral()
{
  return ARandomNumberGenerator::get().nextRange(char('9') + 1, char('0'));
}

#endif //INCLUDED__ATextGenerator_HPP__

