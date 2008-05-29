/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ANumber_HPP__
#define INCLUDED__ANumber_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"

class AOutputBuffer;

#define NUMBER_DEFAULT_PRECISION 20

/*!
Arbitrary length number class
Object initializes to 0
*/
class ABASE_API ANumber : public ASerializable, public ADebugDumpable
{
public:
  //! Special number constructors
  enum ANumberSpecial {
    Infinity,
    NAN
  };

public:
  /*!
  ctor, ~dtor, cctor, cop=
  */
  ANumber()                                  { _clearNumber(); }
  ANumber(const ANumber& nThat)              { operator=(nThat); }
  explicit ANumber(ANumberSpecial eNum);
  explicit ANumber(const AString& strSource) { operator=(strSource); }
  explicit ANumber(int source)               { operator=(source); }
  explicit ANumber(s4 source)                { operator=(source); }
  explicit ANumber(u4 source)                { operator=(source); }
  explicit ANumber(double source)            { operator=(source); }

  /*!
  Copy operators, precisioon copies only when another ANumber object is being copied
  */
  ANumber& operator=(const ANumber& nThat);
  ANumber& operator=(const AString& strSource) { setNumber(strSource); return *this; }
  ANumber& operator=(int source)               { setNumber(AString::fromInt(source)); return *this; }
  ANumber& operator=(s4 source)                { setNumber(AString::fromS4(source)); return *this; }
  ANumber& operator=(u4 source)                { setNumber(AString::fromU4(source)); return *this; }
  ANumber& operator=(double source);

  /*!
  Destructor
  */
  ~ANumber() {}
  
  /*!
  Set to 0
  */
  void clear() { _clearNumber(); }
  
  /*!
  Setting/Parsing
  */
  void cleanNumber(void) { if (!isInfinite() && !isNAN()) _cleanString(mstr__Number); }
  
  /*!
  Given a string (and an optional new precision) this method will parse
    the number and store it internally, supports integer/float looking strings and
   nnnn.nnnnnE+-eeeee scientific notation as well
  */
  void setNumber(const AString& strSource, int iPrecision = -1);
  
  /*!
  Precision (force will pad with '0' if needed)
  */
  void setPrecision(const int iP);
  void forcePrecision(const int iP = -1);        //a_-1 means pad the current number with 0s using the current precision so with precision of 4, 1.3 will become 1.3000

  /*!
  Properties
  */
  void setInfinite()   { mstr__Number = sstr_Infinity; }
  void setNAN()        { mstr__Number = sstr_NAN; }
  void toggleSign()    { mc__Sign = (mc__Sign == '+' ? '-' : '+'); }
  void forcePositive() { mc__Sign = '+'; }
  void forceNegative() { mc__Sign = '-'; }
  void forceWhole()    { mstr__Number.truncateAt('.'); }

  /*!
  Checking
  */
  bool isInfinite() const { return (mstr__Number == ANumber::sstr_Infinity ? true : false); }
  bool isNAN() const      { return (mstr__Number == ANumber::sstr_NAN ? true : false); }
  bool isNegative() const { return (mc__Sign == '+' ? false : true); }
  bool isWhole() const;            //a_Checks if this object is a whole number
  bool isZero() const;             //a_Fast comparison for processor intensive algoroithms
  bool isPrime() const;            //a_Checks primeness of a number (rigorous check at this moment, may not be the quickest)

  /*!
  Unary operators
  */
  ANumber& operator +=(const ANumber& nThat);
  ANumber& operator *=(const ANumber& nThat);
  ANumber& operator -=(const ANumber& nThat);
  ANumber& operator /=(const ANumber& nThat);
  ANumber& power(const ANumber& nThat);
  
  /*!
  Unary power of 10 shift, decimal point shift in direction of arrows
  */
  ANumber& shiftDecimalRight(const int iShift);
  ANumber& shiftDecimalLeft(const int iShift);

  /*!
  Sign operators
  */
  ANumber& operator +(void) { return *this; }
  ANumber& operator -(void) { if (mc__Sign == '-') mc__Sign = '+'; else mc__Sign = '-'; return *this; }

  //! Prefix ++nX
  ANumber& operator ++(void) { *this += ANumber(1); return *this; }
  //! Prefix --nX
  ANumber& operator --(void) { *this -= ANumber(1); return *this; }
  //! Postfix nX++
  ANumber  operator ++(int)  { ANumber nRet(*this); *this += ANumber(1); return nRet; }
  //! Postfix nX--
  ANumber  operator --(int)  { ANumber nRet(*this); *this -= ANumber(1); return nRet; }

  /*!
  Binary operators
  */
  ANumber operator +(const ANumber& nThat) const;
  ANumber operator *(const ANumber& nThat) const;
  ANumber operator -(const ANumber& nThat) const;
  ANumber operator /(const ANumber& nThat) const;

  /*!
  Comparative operators
  */
  bool operator ==(const ANumber& nThat) const;
  bool operator !=(const ANumber& nThat) const;
  bool operator  >(const ANumber& nThat) const;
  bool operator  <(const ANumber& nThat) const;
  bool operator >=(const ANumber& nThat) const;
  bool operator <=(const ANumber& nThat) const;

  /*!
  Helper methods (precision of -1 means use 'double' precision)
  */
  static ANumber factorial(const ANumber &nSource);                    //a_Factorial=(N * (N-1) * (N-2) * ... * 1)
  static ANumber e(const ANumber &nSource, int iPrecision = -0x1);     //a_e ^ N
  static ANumber sin(const ANumber &nSource, int iPrecision = -0x1);   //a_sine of N where N is in radians
  static ANumber cos(const ANumber &nSource, int iPrecision = -0x1);   //a_cosine of N where N is in radians
  
  //a_Functions
  ANumber& sqrt();              //a_Square root, returns reference to self, uses involution method (long hand) to calculate arbitrary length result

  /*!
  I/O methods
  */
  virtual void emit(AOutputBuffer&) const;     //a_Append
  AString toAString() const;                   //a_Convert to AString
  
  /*!
  ASerializable
  */
  virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AString  mstr__Number;                 //a_Number sans sign and exponent
  char     mc__Sign;                     //a_Sign of the number (+/- only)
  u4       m__Precision;                 //a_Precision used during division

  //a_Contains the valid characters in a number
  static const AString smstr__ValidChars;     

//a_UNCOMMENT 3 LINES BELOW ONLY FOR TESTING PURPOSE
//public:
//#pragma message("public: used for testing only, remove when done...")
//#define __UT_ANUMBER_INTERNAL__

  /*! 
  R = A + B
  boolAdjustPrecision is used to prevent adjustment during internal adds
  */
  void _strAdd(const AString& strA, const AString& strB, AString& strR, bool boolAdjustPrecision = true);
  
  /*!
  R = A - B
  Returns non-zero if result is negative
  */
  int _strSubtract(const AString& strA, const AString& strB, AString& strR);


  /*!
  R = A * B
  */
  void _strMultiply(const AString& strA, const AString& strB, AString& strR);

  /*!
  R = A / B
  */
  void _strDivide(const AString& strA, const AString& strB, AString& strR, int iDesiredPrecision = 2);

  /*!
  R = A ^ B
  */
  void _strPower(const AString& strA, const AString& strB, AString& strR);


  /*!
  Compare A and B: >0 if A>B; ==0 if A == B; <0 if A<B
  */
  int _strCompare(const AString& strA, const AString& strB) const;


  /*!
  Raises to the power of 10 (left and right shift in Base-10 world)
  */
  void _strPow10(AString& strA, int iShift = 1);


  /*!
  Worker functions
  */
  void _clearNumber(void);
  void _cleanString(AString& strR, bool boolSpaceOnly = false);   //a_Removes leading spaces and zeros, if string ends up empty sets it to "0"
  void _fixPrecision(AString& strR, int iForce = 0);              //a_Adjusts precision

public:
  /*!
  String constants
  */
  static const AString sstr_NumberWhiteSpace;
  static const AString sstr_Infinity;
  static const AString sstr_NAN;
};

#endif //INCLUDED__ANumber_HPP__
