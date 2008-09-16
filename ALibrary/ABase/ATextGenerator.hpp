/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ATextGenerator_HPP__
#define INCLUDED__ATextGenerator_HPP__

#include "apiABase.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AString.hpp"

class ABASE_API ATextGenerator
{
public:
  /*!
  Generate random numerals and append to target
  
  @param target to append result to
  @param length of the resulting random string that will get appended
  */
  static void generateRandomNumber(AOutputBuffer& target, size_t length = 1);
  
  /*!
  Generate random alphanum and append to target
  Upper and lower case letters and numerals

  @param target to append result to
  @param length of the resulting random string that will get appended
  */
  static void generateRandomAlphanum(AOutputBuffer& target, size_t length = 1);
  
  /*!
  Generate from template

  Template characters
   \verbatim
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
  \endverbatim

  @param target to append result to
  @param strTemplate contains the template to use for generating random data
  */
  static void generateFromTemplate(AOutputBuffer& target, const AString &strTemplate);
  
  /*!
  Reset the counter used in templates when calling generateFromTemplate
  Only affects %z and nothing else (everything else is random)
  This method should be called as a way to initialize the counter if it should not be 0 at startup
  
  NOTE: Not thread safe, use synchronization around this method and generateFromTemplate if calling this from more than 1 thread

  @param counterValue to set the current counter that will be evaluated when %z is used in generateFromTemplate
  */
  static void resetCounter(int counterValue = 0);

  /*!
  Quasi-random word generator

  @param target to append result to
  @param length of the resulting random string that will get appended
  */
  static void generateRandomWord(AOutputBuffer& target, size_t length = 1);

  /*!
  Random string based on a charset

  @param target to append result to
  @param length of the resulting random string that will get appended
  @param charset to use when generating the random string
  */
  static void generateRandomString(AOutputBuffer& target, size_t length, const AString& charset = AConstant::CHARSET_URLSAFE_ALPHANUM);

  /*!
  Random string for [0,0xFF] (8 random bits per character, everything including 0)

  Appends length number of random 8-bit values to the target specified

  @param target to append result to
  @param length of the resulting random string that will get appended
  */
  static void generateRandomData(AOutputBuffer& target, size_t length);

  /*!
  Unique identifier based on current time and random number
  Not guaranteed to be unique across processes (but extremely likely to be)
  For guaranteed results use machine specific GUID generator functions
  maxSize must be >16 characters long
  Appends to target up to maxSize

  @param target to append result to
  @param maxSize more than 16, specificies the final length of the unique string appended to target
  */
  static void generateUniqueId(AOutputBuffer& target, size_t maxSize = 32);

  /*!
  Random lowercase letter [a,z]
  @return random lowercase letter
  */
  static inline char generateRandomLowercaseLetter();

  /*!
  Random uppercase letter [A,Z]
  @return random lowercase letter
  */
  static inline char generateRandomUppercaseLetter();

  /*!
  Random numeral [0,9]
  @return random lowercase letter
  */
  static inline char generateRandomNumeral();

private:
  //! Count used by %z parameter
  static int si_Counter;
};

inline char ATextGenerator::generateRandomLowercaseLetter()
{
  return (char)ARandomNumberGenerator::get().nextRange(char('z') + 1, char('a'));
}
inline char ATextGenerator::generateRandomUppercaseLetter()
{
  return (char)ARandomNumberGenerator::get().nextRange(char('Z') + 1, char('A'));
}
inline char ATextGenerator::generateRandomNumeral()
{
  return (char)ARandomNumberGenerator::get().nextRange(char('9') + 1, char('0'));
}

#endif //INCLUDED__ATextGenerator_HPP__

