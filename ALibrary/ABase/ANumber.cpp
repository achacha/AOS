/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ANumber.hpp"
#include "AFile.hpp"
#include "AOutputBuffer.hpp"
#include <float.h>

void ANumber::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  //a_Members
  ADebugDumpable::indent(os, indent+1) << "mstr__Number=" << mstr__Number.c_str() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mc__Sign=" << mc__Sign;
  ADebugDumpable::indent(os, indent+1) << "m__Precision=" << m__Precision << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

//a_STATICs
//a_Valid chars
const AString ANumber::smstr__ValidChars("0123456789.-+Ee,");

//a_Easy to use string constant
const AString ANumber::sstr_NumberWhiteSpace(" 0");
const AString ANumber::sstr_Infinity("Inf");
const AString ANumber::sstr_NAN("NaN");

ANumber::ANumber(ANumberSpecial eNum)
{
  _clearNumber();
  switch (eNum)
  {
    case Infinity :
      mstr__Number = ANumber::sstr_Infinity;
    break;

    case NAN :
      mstr__Number = ANumber::sstr_NAN;
    break;

    default:
      ATHROW(this, AException::InvalidParameter);
  }
}

AString ANumber::toAString() const
{ 
  AString strR; 
  if (isNegative()) strR = mc__Sign; 
  strR += mstr__Number; 
  return strR;
}

void ANumber::emit(AOutputBuffer& target) const
{ 
  if (isNegative()) target.append(mc__Sign); 
  target.append(mstr__Number);
}

void ANumber::setPrecision(const int iP)
{ 
  if (iP >= 0)
    m__Precision = iP;
  else 
    ATHROW(this, AException::NegativePrecision);
  
  _fixPrecision(mstr__Number);
}

void ANumber::forcePrecision(const int iP)
{ 
  //a_Change precision if >0, else use _fixPrecision call with current precision
  if (iP >= 0)
    m__Precision = iP;
 
  _fixPrecision(mstr__Number, 0x1);
}

void ANumber::_clearNumber(void)
{
  mstr__Number = AConstant::ASTRING_ZERO;
  mc__Sign = '+';
  m__Precision = NUMBER_DEFAULT_PRECISION;
}

void ANumber::_cleanString(AString& strR, bool boolSpaceOnly)
{
  //a_Check and ignore "0.XXX" format
  if (strR.getSize() > 2 && strR.peek(0) == '0' && strR.peek(1) == '.')
  {
    //Nothing yet
  }
  else
  {
    //a_Clean up leading white space and zeros
    strR.stripLeading(ANumber::sstr_NumberWhiteSpace);
  }

  //a_Remove trailing garbage
  if (!boolSpaceOnly)
  {
    if (strR.find('.') != AConstant::npos)
      strR.stripTrailing(AConstant::ASTRING_ZERO);
    strR.stripTrailing(AConstant::ASTRING_PERIOD);
  }

  if (strR.isEmpty()) strR = AConstant::ASTRING_ZERO;       //a_A zero
  else if (strR.peek(0) == '.')
    strR = AString('0') + strR;                //a_Convert .XXX to 0.XXX
}

void ANumber::_fixPrecision(AString& strR, int iForce)
{
  u4 decimal = strR.find('.');

  if (iForce || ((decimal != AConstant::npos) && strR.getSize() > m__Precision + decimal + 0x1))
  {
    if (decimal == AConstant::npos && iForce)
    {
      strR += '.';  //a_Append '.' for precision forcing
      decimal = strR.getSize() - 0x1;
    }

    //a_Need to trucate (and remove '.' if the new precision is 0)
    u4 iNewSize = decimal + m__Precision + (m__Precision == 0 ? 0x0 : 0x1);
    if (iNewSize < strR.getSize())
    {
      //a_Rounding
      char cLast = strR.peek(iNewSize);
      strR.setSize(iNewSize, '0');
      if (cLast > '4')
      {
        AString strAdd(AConstant::ASTRING_ONE);
        int p = m__Precision;
				_strPow10(strAdd, -p);
        _strAdd(AString(strR), strAdd, strR, false);
      }
    }
    else
      strR.setSize(iNewSize, '0');
  }
}

bool ANumber::isZero(void) const
{ 
  if (isInfinite() || isNAN()) return false;   //a_Infinity/NAN is not zero

  //a_Remove precision forced zeros
  ANumber nTest(*this);
  nTest.cleanNumber();

  return (nTest.mstr__Number == AConstant::ASTRING_ZERO ? true : false);
}


bool ANumber::isWhole(void) const
{ 
  if (isInfinite())
    ATHROW(this, AException::InfinityUnexpected);

  if (isNAN())
    ATHROW(this, AException::NANUnexpected);

  return (mstr__Number.find('.') == AConstant::npos ? true : false);
}
 
bool ANumber::isPrime() const
{
  //a_Zero is prime
  if (isZero())
    return true;

  //a_Infitity is undefined
  if (isInfinite())
    ATHROW(this, AException::InfinityUnexpected);

  //a_Infitity is undefined
  if (isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Irrational and negative numbers are not prime
  if (!isWhole() || isNegative())
    return false;

  //a_The only even prime number
  if (!mstr__Number.compare("2"))
    return true;

  //a_Primes can only have last digit of 1,3,7,9 if >10
  switch(mstr__Number.at(mstr__Number.getSize()-1))
  {
    case '0':
    case '2':
    case '4':
    case '5':
    case '6':
    case '8':
      return false;
  }

  ANumber divisor(3);
  ANumber endnum(*this);
  endnum.sqrt();

  for (;divisor < endnum; ++divisor)
  {
    ANumber quotient(*this / divisor);
    if (quotient.isWhole())
      return false;
  }

  return true;
}

ANumber& ANumber::operator=(const ANumber& nThat)
{ 
  if (this != &nThat) 
  {
    mstr__Number = nThat.mstr__Number;
    mc__Sign = nThat.mc__Sign;
    m__Precision = nThat.m__Precision;
  }
  return *this;
} 

void ANumber::setNumber(const AString& strSource, int iPrecision)
{
  AString strWork(strSource);
  strWork.stripTrailing(AConstant::ASTRING_WHITESPACE);
  strWork.stripLeading(AConstant::ASTRING_WHITESPACE);

  _clearNumber();
  if (strWork.isEmpty())
    return;

  u4 iLength;
  if ((iLength = strWork.getSize()) < 0x0)
    ATHROW(this, AException::InvalidObject);
  
  //a_First validate that it is a number with valid character
  //a_Can contain Ee to denote exponentials that will be expanded
  //a_(,) commas will be removed and (+/-) will be dropped and stored elsewhere
  if (strSource.findNotOneOf(smstr__ValidChars) != AConstant::npos)
    ATHROW(this, AException::InvalidCharacter);

  //a_Varibales for temporary storage while parsing is done
  u4 iX = 0x0, iStart = 0x0, iEnd = AConstant::npos;
  char cSign = '+', cExponentSign = '+';
	u4 decimal = AConstant::npos, exponent = AConstant::npos;
  
  //a_Determine if a sign was used
  if (strSource.peek(0) == '-' || strSource.peek(0) == '+')
  {
    cSign = strSource.peek(0);         //a_Save sign in temporary char until the end
    iX++;                              //a_Next
    iStart = 0x1;
  }

  //a_To contain no more that 1 decimal (and save the position)
  //a_Save exponent position
  for (; iX < iLength; iX++)
  {
    switch(strSource.peek(iX))
    {
      case '.' : //a_Decimal place found
				if (AConstant::npos != decimal)
          ATHROW(this, AException::TooManyDecimalPoints);
        else
          decimal = iX++;
      break;

      case 'E' : case 'e' : //a_Exponent specified
        if (AConstant::npos != exponent)
          ATHROW(this, AException::TooManyExponentSpecifiers);
        else
        {
          iEnd = iX;              //a_End of number begining of the exponent
          exponent = iX++;
        }
      break;

      default : iX++;
    }
  }

  //a_If end was not found, then it is the literal end of the string
  if (iEnd == AConstant::npos)
    iEnd = iLength;


  //a_Leading decimal
  if (decimal == iStart && decimal >= 0)
  {
    //a_Now extract the number out
    mstr__Number.assign(AConstant::ASTRING_ZERO);
    strSource.peek(mstr__Number, iStart, iEnd - iStart);
  }
  else
  {
    //a_Now extract the number out
    strSource.peek(mstr__Number, iStart, iEnd - iStart);
  }

  AString strExponent(AConstant::ASTRING_ZERO);
  //a_Extract exponent (to 10^exponent it)
  if (exponent > 0x0 && exponent < strSource.getSize())
  {
    strExponent = (cExponentSign == '-' ? AString("-") : AConstant::ASTRING_EMPTY);
    strSource.peek(strExponent, exponent + 0x1);
  }

  //a_Now we have our number and exponent
  _strPow10(mstr__Number, atoi(strExponent.c_str()));

  //a_Finalize the parsing of this object
  mc__Sign = cSign;
  if (iPrecision >= 0x0) setPrecision(iPrecision);
  else
  {
    //a_We have our string, let's find the precision we need
    if ((decimal = mstr__Number.find('.')) != AConstant::npos)
      if (mstr__Number.getSize() - decimal - 1 > DBL_DIG)
        m__Precision = mstr__Number.getSize() - decimal - 1;
  }
}


ANumber& ANumber::operator +=(const ANumber& nThat)
{
  if (isNAN() || nThat.isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Handling of infinity
  if (isInfinite()) return *this;     //a_Infinity + anything = Infinity
  if (nThat.isInfinite())
  {
    setInfinite();
    return *this;
  }

  //a_Take the higher precision
  if (nThat.m__Precision > m__Precision) setPrecision(nThat.m__Precision);

  //a_Addition in mixed signs
  if (mc__Sign == '-')
  {
    if (nThat.mc__Sign == '-')
    {
      //a_ -this + -that
      _strAdd(AString(mstr__Number), nThat.mstr__Number, mstr__Number);
    }
    else
    {
      //a_ -this + +that
      switch(_strCompare(mstr__Number, nThat.mstr__Number))
      {
        case -1 : 
          if (_strSubtract(nThat.mstr__Number, AString(mstr__Number), mstr__Number))
            mc__Sign = '-';
          else
            mc__Sign = '+';
        break;
        
        case  0 : mstr__Number = AConstant::ASTRING_ZERO; mc__Sign = '+'; break;

        case  1 :
          if (_strSubtract(AString(mstr__Number), nThat.mstr__Number, mstr__Number))
            mc__Sign = '+';
          else
            mc__Sign = '-';
         break;

        default : ATHROW(this, AException::InvalidReturnValue);
      }
    }
  }
  else
  {
    if (nThat.mc__Sign == '-')
    {
      //a_ +this + -that
      switch(_strCompare(mstr__Number, nThat.mstr__Number))
      {
        case -1 : 
          if (_strSubtract(nThat.mstr__Number, AString(mstr__Number), mstr__Number))
            mc__Sign = '+';
          else
            mc__Sign = '-';
        break;

        case  0 : mstr__Number = AConstant::ASTRING_ZERO; mc__Sign = '+'; break;

        case  1 : 
          if (_strSubtract(AString(mstr__Number), nThat.mstr__Number, mstr__Number))
            mc__Sign = '-';
          else
            mc__Sign = '+';
        break;

        default : ATHROW(this, AException::InvalidReturnValue);
      }
    }
    else
    {
      //a_ +this + +that
      _strAdd(nThat.mstr__Number, AString(mstr__Number), mstr__Number);
    }
  }

  return *this;
}


ANumber& ANumber::operator -=(const ANumber& nThat)
{
  //a_Not a number
  if (isNAN() || nThat.isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Undefined cases
  if (isInfinite() && nThat.isInfinite())
    ATHROW(this, AException::InfinityUnexpected);

  //a_Handling of infinity
  if (isInfinite()) return *this;     //a_Infinity - Anything = Infinity
  if (nThat.isInfinite())
  {
    setInfinite();
    mc__Sign = '-';
    return *this;
  }

  //a_Take the higher precision
  if (nThat.m__Precision > m__Precision) setPrecision(nThat.m__Precision);

  //a_Subtraction in mixed signs
  if (mc__Sign == '-')
  {
    if (nThat.mc__Sign == '-')
    {
      //a_ -this - -that
      switch(_strCompare(mstr__Number, nThat.mstr__Number))
      {
        case -1 : 
          if (_strSubtract(nThat.mstr__Number, AString(mstr__Number), mstr__Number))
            mc__Sign = '-';
          else
            mc__Sign = '+';
        break;

        case  0 : mstr__Number = AConstant::ASTRING_ZERO; mc__Sign = '+'; break;

        case  1 : 
          if (_strSubtract(AString(mstr__Number), nThat.mstr__Number, mstr__Number))
            mc__Sign = '+';
          else
            mc__Sign = '-';
        break;

        default : ATHROW(this, AException::InvalidReturnValue);
      }
    }
    else
    {
      //a_ -this - +that
      _strAdd(AString(mstr__Number), nThat.mstr__Number, mstr__Number);
    }
  }
  else
  {
    if (nThat.mc__Sign == '-')
    {
      //a_ +this - -that
      _strAdd(nThat.mstr__Number, AString(mstr__Number), mstr__Number);
    }
    else
    {
      //a_ +this + +that
      switch(_strCompare(mstr__Number, nThat.mstr__Number))
      {
        case -1 : 
          if (_strSubtract(nThat.mstr__Number, AString(mstr__Number), mstr__Number))
            mc__Sign = '+';
          else
            mc__Sign = '-';
        break;

        case  0 : mstr__Number = AConstant::ASTRING_ZERO; mc__Sign = '+'; break;

        case  1 : 
          if (_strSubtract(AString(mstr__Number), nThat.mstr__Number, mstr__Number))
            mc__Sign = '-';
          else
            mc__Sign = '+'; break;

        default : ATHROW(this, AException::InvalidReturnValue);
      }
    }
  }

  return *this;
}


ANumber& ANumber::operator *=(const ANumber& nThat)
{
  //a_Not a number
  if (isNAN() || nThat.isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Undefined cases
  if (isZero() && nThat.isInfinite())
    ATHROW(this, AException::ZeroTimesInfinity);

  if (isInfinite() && nThat.isZero())
    ATHROW(this, AException::InfinityTimesZero);

	//a_Multiplication by zero
  if (nThat.isZero() || mstr__Number  == AConstant::ASTRING_ZERO)
  {
    mstr__Number = AConstant::ASTRING_ZERO;
    return *this;
  }

  //a_Handling of infinity
  if (isInfinite()) return *this;     //a_Infinity * non-zero number = Infinity
  if (nThat.isInfinite())
  {
    setInfinite();
    return *this;
  }

  //a_Take the higher precision
  if (nThat.m__Precision > m__Precision) setPrecision(nThat.m__Precision);

  //a_Multiplication
  _strMultiply(AString(mstr__Number), nThat.mstr__Number, mstr__Number);
  if (nThat.mc__Sign == '-')
  {
    if (mc__Sign == '-')
      mc__Sign = '+';
    else
      mc__Sign = '-';
  }

  return *this;
}


ANumber& ANumber::operator /=(const ANumber& nThat) 
{
  //a_Not a number
  if (isNAN() || nThat.isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Undefined cases
  if (isInfinite() && nThat.isInfinite())
    ATHROW(this, AException::InfinityOverInfinity);

  if (isZero() && nThat.isZero())
    ATHROW(this, AException::ZeroOverZero);

  if (isZero() && nThat.isInfinite())
    ATHROW(this, AException::ZeroOverInfinity);

  if (isInfinite() && nThat.isZero())
    ATHROW(this, AException::InfinityOverZero);

	//a_Division by zero
  if (nThat.isZero())
  {
    setInfinite();
    return *this;
  }

  //a_Zero divided by number
  if (isZero())
  {
    mstr__Number = AConstant::ASTRING_ZERO;
    return *this;
  }

  //a_Infinity divided by number
  if (isInfinite())
  {
    return *this;     //a_Infinity / non-zero number = Infinity
  }

  //a_Division by infinity
  if (nThat.isInfinite())
  {
    mstr__Number = AConstant::ASTRING_ZERO;
    return *this;
  }

  //a_Take the higher precision
  if (nThat.m__Precision > m__Precision) setPrecision(nThat.m__Precision);

  //a_Division
  _strDivide(AString(mstr__Number), nThat.mstr__Number, mstr__Number, m__Precision);
  if (nThat.mc__Sign == '-')
  {
    if (mc__Sign == '-')
      mc__Sign = '+';
    else
      mc__Sign = '-';
  }

  return *this;
}


ANumber& ANumber::power(const ANumber& nThat)
{
  //a_Not a number
  if (isNAN() || nThat.isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Negative power not supported
  if (nThat.mc__Sign == '-')
    ATHROW(this, AException::NotSupported);
  
  //a_Power of zero
  if (isZero())
  {
    mstr__Number = AConstant::ASTRING_ZERO;
    return *this;
  }

  //a_To power zero
  if (nThat.isZero())
  {
    mstr__Number = AConstant::ASTRING_ONE;
    return *this;
  }

  //a_To power zero
  if (!nThat.isWhole())
    ATHROW(this, AException::NotSupported);

  //a_Handling of infinity
  if (isInfinite()) return *this;     //a_Infinity ^ non-zero number = Infinity
  if (nThat.isInfinite())             //a_non-zero ^ Infinity = Infinity
  {
    setInfinite();
    return *this;
  }

  //a_Take the higher precision
  if (nThat.m__Precision > m__Precision) setPrecision(nThat.m__Precision);

  //a_Power (sign unchanged)
  _strPower(AString(mstr__Number), nThat.mstr__Number, mstr__Number);

  if (
       (mc__Sign == '-') &&
       (nThat > ANumber("0")) &&
       ((nThat / ANumber("2")).isWhole())
     )
  {
    //a_Toggle sign
    mc__Sign = '+';
  }

  return *this;
}

ANumber& ANumber::shiftDecimalRight(const int iShift)
{
  //a_Not a number
  if (isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Zero/Infinity cases
  if (isZero() || iShift == 0 || isInfinite())
    return *this;     //a_Zero shifted is still a zero, shift zero times is the number itself
    
  //a_Preform the power!
  _strPow10(mstr__Number, iShift);

  return *this;
}

ANumber& ANumber::shiftDecimalLeft(const int iShift)
{
  //a_Not a number
  if (isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Infinity
  if (isInfinite())
    ATHROW(this, AException::InfinityUnexpected);

  //a_Zero/Infinity cases
  if (isZero() || iShift == 0 || isInfinite())
    return *this;     //a_Zero shifted is still a zero, shift zero times is the number itself
    
  //a_Take the higher precision
  u4 decimal_a = mstr__Number.find('.');
  int a1 = (decimal_a == AConstant::npos ? 0x0 : mstr__Number.getSize() - decimal_a - 0x1);
  if (iShift + a1 > (int)m__Precision) setPrecision(iShift + a1);

  //a_Preform the power!
  _strPow10(mstr__Number, -iShift);

  return *this;
}

ANumber ANumber::operator +(const ANumber& nThat) const
{
  ANumber nWork(*this);

  //a_Preform addition
  nWork += nThat;

  return nWork;
}

ANumber ANumber::operator *(const ANumber& nThat) const
{
  ANumber nWork(*this);

  //a_Preform multiplication
  nWork *= nThat;

  return nWork;
}

ANumber ANumber::operator -(const ANumber& nThat) const
{
  ANumber nWork(*this);
  
  //a_Preform subtraction
  nWork -= nThat;
  
  return nWork;
}

ANumber ANumber::operator /(const ANumber& nThat) const
{
  ANumber nWork(*this);

  //a_Preform division
  nWork /= nThat;

  return nWork;
}

bool ANumber::operator ==(const ANumber& nThat) const
{
  if (_strCompare(mstr__Number, nThat.mstr__Number) == 0x0 && mc__Sign == nThat.mc__Sign)
    return true;

  return false;
}

bool ANumber::operator !=(const ANumber& nThat) const
{
  if (_strCompare(mstr__Number, nThat.mstr__Number) == 0x0 && mc__Sign == nThat.mc__Sign)
    return false;

  return true;
}

bool ANumber::operator >(const ANumber& nThat) const
{
  //a_Not a number
  if (isNAN() || nThat.isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Infinity
  if (isInfinite() || nThat.isInfinite())
    ATHROW(this, AException::InfinityUnexpected);

  //a_Sign case
  if (mc__Sign == '+' && nThat.mc__Sign == '-') return true;
  if (mc__Sign == '-' && nThat.mc__Sign == '+') return false;

  //a_Sign is the same
  int iComp = _strCompare(mstr__Number, nThat.mstr__Number);
  if (( iComp > 0x0 && mc__Sign == '+') || (iComp < 0x0 && mc__Sign == '-'))
    return true;

  return false;
}

bool ANumber::operator <(const ANumber& nThat) const
{
  //a_Not a number
  if (isNAN() || nThat.isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Infinity
  if (isInfinite() || nThat.isInfinite())
    ATHROW(this, AException::InfinityUnexpected);

  //a_Sign case
  if (mc__Sign == '-' && nThat.mc__Sign == '+') return true;
  if (mc__Sign == '+' && nThat.mc__Sign == '-') return false;

  //a_Sign is the same
  int iComp = _strCompare(mstr__Number, nThat.mstr__Number);
  if (( iComp < 0x0 && mc__Sign == '+') || (iComp > 0x0 && mc__Sign == '-'))
    return true;

  return false;
}

bool ANumber::operator >=(const ANumber& nThat) const
{
  return !(operator <(nThat));
}

bool ANumber::operator <=(const ANumber& nThat) const
{
  return !(operator >(nThat));
}

// Long form square root (Inversion)
//
//                            result
//                          ------------
//  acc(n) = (2x result)_n  | source
//                                   
// source = "a0.a1"
ANumber& ANumber::sqrt()
{
  if (mc__Sign == '-')
    ATHROW(this, AException::NegativeSquareRoot);

  //a_Not a number
  if (isNAN())
    ATHROW(this, AException::NANUnexpected);

  //a_Infinity
  if (isInfinite())
    ATHROW(this, AException::InfinityUnexpected);
  
  if (isZero() || !_strCompare(mstr__Number, AConstant::ASTRING_ONE))
    return *this;        //a_Square root of zero and one is zero and one

  AString strSource(mstr__Number);
  AString strWork(m__Precision * 2 + 8, 128);
  AString strResult(m__Precision * 2 + 8, 128);
  AString strAcc(4, 4);
  u4 currentPrecision = 0;
  u4 decimal = strSource.find('.');
  if (decimal == AConstant::npos)
    decimal = strSource.getSize();

  if (decimal & 0x1)
  {
    strWork.assign(strSource, 1);
    strSource.remove(1);
  }
  else
  {
    strWork.assign(strSource, 2);
    strSource.remove(2);
  }

  AString strTemp(m__Precision * 2 + 8, 128);
  AString strTempAcc(m__Precision + 4, 128);
  AString strAccAppend(4, 4);
  AString strResultNoDot(m__Precision + 4, 128);
  AString strProduct(m__Precision * 2 + 8, 128);
  bool boolPeriodAdded = false;

  //a_Initial result
  strResultNoDot.assign(strResult);

  //a_Accumulator with 5 appended will be used to test division of the work string, adjusted up/down in a loop
  strTempAcc = strAcc;
  strTempAcc.append('5');

  //a_Create initial accumulator
  bool boolLoop = true;
  while (boolLoop)
  {
    //a_Determine accumulator add-on digit
    strAccAppend.assign('5');     //a_The unknown digit of accumulator will start at 5 and go up or down
    _strMultiply(strTempAcc, strAccAppend, strTemp);
    AString strAccAppendLast;

    int compareResult = _strCompare(strWork, strTemp);
    if (compareResult > 0)
    {
      //a_Increase
      do
      {
        strAccAppendLast = strAccAppend;
        strTempAcc = strAcc;
        _strAdd(strAccAppend, AConstant::ASTRING_ONE, strTemp);
        strTempAcc.append(strTemp);
        strAccAppend = strTemp;
        _strMultiply(strTempAcc, strAccAppend, strTemp);
      }
      while (_strCompare(strWork, strTemp) >= 0);
      strAccAppend = strAccAppendLast;       //a_Last value before overflow
    }
    else if (compareResult < 0)
    {
      //a_Decrease
      do
      {
        strAccAppendLast = strAccAppend;
        strTempAcc = strAcc;
        _strSubtract(strAccAppend, AConstant::ASTRING_ONE, strTemp);
        strTempAcc.append(strTemp);
        strAccAppend = strTemp;
        _strMultiply(strTempAcc, strAccAppend, strTemp);
      }
      while (_strCompare(strWork, strTemp) <= 0);
    }

    //a_Subtract Acc and AccAppend from Work
    strAcc.append(strAccAppend);
    _strMultiply(strAcc, strAccAppend, strProduct);
    _strSubtract(strWork, strProduct, strWork);

    //a_Update Acc
    strResult.append(strAccAppend);
    
    //a_Breakout conditions
    if (currentPrecision > m__Precision+1)
      break;    //a_Break out if one over precision, so we can round
    if (!_strCompare(strWork, AConstant::ASTRING_ZERO) && strSource.getSize() == 0)
      break;    //a_Perfect square, have result

    //a_Generate new Acc for testing in the loop ahead
    strResultNoDot.append(strAccAppend);
    _strAdd(strResultNoDot, strResultNoDot, strAcc); 
    strTempAcc = strAcc;
    strTempAcc.append('1');

    //a_Expand the work string
    bool boolAddZeroToResult = false;
    while (_strCompare(strWork, strTempAcc) < 0)
    {
      if (!strSource.isEmpty())
      {
        if (strSource.at(0) == '.')
        {
          boolPeriodAdded = true;
          strResult.append(AConstant::ASTRING_PERIOD);
          strSource.remove();
          if (strSource.getSize() & 0x1)
            strSource.append('0');       //a_Even # of digits
        }
        else
        {
          strSource.get(strTemp, 0, 2);
          strWork.append(strTemp);

          //a_Next time we need to add zero to result
          if (boolAddZeroToResult)
          {
            strResult.append('0');
            strResultNoDot.append('0');
            strAcc.append('0');

            //a_Update the temporary accumulator
            strTempAcc = strAcc;
            strTempAcc.append('1');
          }
          else
            boolAddZeroToResult = true;
        }
        strWork.stripLeading(AConstant::ASTRING_ZERO);
      }
      else
      {
        if (!boolAddZeroToResult)
        {
          //a_Bring down 00
          strWork.append(AConstant::ASTRING_DOUBLEZERO, 2);
          boolAddZeroToResult = true;
        }
        else
        {
          //a_Additional 00 will add 0 to the result
          strWork.append(AConstant::ASTRING_DOUBLEZERO, 2);
          strResult.append('0');
          strResultNoDot.append('0');
          strAcc.append('0');

          //a_Update the temporary accumulator
          strTempAcc = strAcc;
          strTempAcc.append('1');
        }

        strWork.stripLeading(AConstant::ASTRING_ZERO);
        if (!boolPeriodAdded)
        {
          if (strWork.isEmpty() && strSource.getSize() == 0)
          {
            //a_We are done with this number, perfect square
            boolLoop = false;
            break;
          }
          else
          {
            boolPeriodAdded = true;
            strResult.append(AConstant::ASTRING_PERIOD);
          }
        }

        ++currentPrecision;
      }
    }
  }

  //a_Round if needed
  if (currentPrecision > m__Precision)
    _fixPrecision(strResult, m__Precision);
  
  mstr__Number = strResult;
  
  return *this;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//a_   |  A0  | A1 |
//a_   |000000.0000|
//a_          ^   
//a_      DecimalA
void ANumber::_strAdd(const AString& strA, const AString& strB, AString& strR, bool boolAdjustPrecision)
{
  u4 decimal_a = strA.find('.'),
     decimal_b = strB.find('.'),
     a1  = (decimal_a == AConstant::npos ? 0x0 : strA.getSize() - decimal_a - 0x1),
     b1  = (decimal_b == AConstant::npos ? 0x0 : strB.getSize() - decimal_b - 0x1),
     a0  = (decimal_a == AConstant::npos ? strA.getSize() : decimal_a),
     b0  = (decimal_b == AConstant::npos ? strB.getSize() : decimal_b);

  AString strXA((decimal_a == AConstant::npos ? strA + AConstant::ASTRING_PERIOD : strA)),
          strXB((decimal_b == AConstant::npos ? strB + AConstant::ASTRING_PERIOD : strB));

  //a_Determine the new needed size (1{decimal point} + 1{possible carry overflow} + max_integer_digits + max_decimal_digits)
  int iNewSize = 2 + (a0 > b0 ? a0 : b0) + (a1 > b1 ? a1 : b1);

  strR.clear();
  strR.setSize(iNewSize, ' ');

  //a_Equalize the decimal precision
  if (a1 > b1)
      strXB.setSize(strXB.getSize() + a1 - b1, '0');
  else
    if (a1 < b1)
      strXA.setSize(strXA.getSize() + b1 - a1, '0');

  //a_Add the fractional part
  u4 uA = strXA.getSize() - 0x1,
     uB = strXB.getSize() - 0x1,
     uR = strR.getSize() - 0x1;

  //a_Check if both have a decimal at the end
  if (strXA.peek(uA) == '.' && strXB.peek(uB) == '.')
  { 
    uA--;
    uB--; 
  }

  //a_Onward to the addition...
  char cA, cB, cCarry = '\x0';
  bool boolA = false, boolB = false;
  do
  {
    if (strXA.peek(uA) == '.')
    {
      strR.use(uR) = '.';
    }
    else
    {
      cA = (boolA ? '\x0' : strXA.peek(uA));
      cB = (boolB ? '\x0' : strXB.peek(uB));
        
      strR.use(uR) = cA + cB - ((cA && cB) ? '0' : '\x0') + cCarry;
      
      cCarry = '\x0';
      if (strR.peek(uR) > '9')
      { 
        strR.use(uR) -= '\xA';
        cCarry = '\x1';
      }
    }
    if (uA) 
      uA--; 
    else 
      boolA = true;
    if (uB)
      uB--;
    else 
      boolB = true;
    uR--;
  } while (!boolA || !boolB);

  if (cCarry)
    strR.use(uR) = '1';

  //a_Cleanup
  if (boolAdjustPrecision)
  {
    _cleanString(strR);
    _fixPrecision(strR);
  }
  else
    _cleanString(strR, true);
}


//a_   |  A0  | A1 |
//a_   |000000.0000|
//a_          ^   
//a_      DecimalA
int ANumber::_strSubtract(const AString& strA, const AString& strB, AString& strR)
{
  AString strXA(strA), strXB(strB);
  int iNegative = 0;

  //a_Clean up leading white space and zeros
  strXA.stripLeading(ANumber::sstr_NumberWhiteSpace);
  strXB.stripLeading(ANumber::sstr_NumberWhiteSpace);

  //a_A is zero, result is -B
  if (strXA.isEmpty())
  {
    strR = strXB;
    return 1;
  }

  //a_Nothing to do, result is A
  if (strXB.isEmpty())
  {
    if (strXA.peek(0x0) == '.')
      strR = AConstant::ASTRING_ZERO + strXA;
    else
      strR = strXA;
    return 0;
  }


  //a_Switch the two if A<B and default to negative outcome
  if (_strCompare(strXA, strXB) < 0)
  {
    //a_Swap strings
    AString strTemp(strXA);
    strXA = strXB;
    strXB = strTemp;

    iNegative = 1;
  }

  u4 decimal_a = strXA.find('.'),
     decimal_b = strXB.find('.'),
     a1  = (decimal_a == AConstant::npos ? 0x0 : strXA.getSize() - decimal_a - 0x1),
     b1  = (decimal_b == AConstant::npos ? 0x0 : strXB.getSize() - decimal_b - 0x1),
     a0  = (decimal_a == AConstant::npos ? strXA.getSize() : decimal_a),
     b0  = (decimal_b == AConstant::npos ? strXB.getSize() : decimal_b);

  //a_String conditioning
  if (decimal_a == AConstant::npos) strXA += '.';
  if (decimal_b == AConstant::npos) strXB += '.';
  if (decimal_a == 0x0) strXA = AConstant::ASTRING_ZERO + strXA;
  if (decimal_b == 0x0) strXB = AConstant::ASTRING_ZERO + strXB;

  //a_Determine the new needed size (Max of the pre/post decimal + 2 decimals)
  int iNewSize = (a0 > b0 ? a0 : b0) + (a1 > b1 ? a1 : b1) + 2;

  strR.clear();
  strR.setSize(iNewSize, ' ');

  //a_Equalize the decimal precision
  if (a1 > b1)
      strXB.setSize(strXB.getSize() + a1 - b1, '0');
  else
    if (a1 < b1)
      strXA.setSize(strXA.getSize() + b1 - a1, '0');

  //a_Add the fractional part
  u4 uA = strXA.getSize() - 0x1,
     uB = strXB.getSize() - 0x1,
     uR = strR.getSize() - 0x1;

  //a_Check if both have a decimal at the end
  if (strXA.peek(uA) == '.' && strXB.peek(uB) == '.')
  { 
    uA--;
    uB--; 
  }

  char cA, cB, cCarry = '\x0';
  bool boolA = false, boolB = false;
  do
  {
    if (strXA.peek(uA) == '.')
    {
      strR.use(uR) = '.';
    }
    else
    {
      cA = (boolA ? '\x0' : strXA.peek(uA));
      cB = (boolB ? '\x0' : strXB.peek(uB));

      strR.use(uR) = cA + (cB ? '0' : '\x0') - cB - cCarry;
      cCarry = '\x0';
      if (strR.peek(uR) < '0')
      { 
        strR.use(uR) += '\xA';
        cCarry = '\x1';
      }
    }
    if (uA)
      uA--;
    else 
      boolA = true;
    if (uB)
      uB--; 
    else 
      boolB = true;
    uR--;
  } while (!boolA || !boolB);

  //a_Cleanup
  _cleanString(strR);
  _fixPrecision(strR);

  return iNegative;  //a_Returns non-zero if the sign of the result is negative
}


void ANumber::_strMultiply(const AString& strA, const AString& strB, AString& strR)
{
  //a_Clean up leading white space and zeros
  AString strXA(strA), strXB(strB);
  _cleanString(strXA);
  _cleanString(strXB);

  //a_Multiplication by 1
  if (strXA == AConstant::ASTRING_ONE)
  { 
    strR = strXB;
    return; 
  }
  if (strXB == AConstant::ASTRING_ONE)
  { 
    strR = strXA;
    return;
  }

  u4 decimal_a = strXA.find('.'),
     decimal_b = strXB.find('.'),
     a1 = (decimal_a == AConstant::npos ? 0x0 : strXA.getSize() - decimal_a - 0x1),
     b1 = (decimal_b == AConstant::npos ? 0x0 : strXB.getSize() - decimal_b - 0x1);

  //a_Remove decimals
  _strPow10(strXA, a1);
  _strPow10(strXB, b1);

  //a_Resize and clean the string
  strR.clear();
  int iSize = strXA.getSize() + strXB.getSize();          //a_Used later to offset the base
  strR.setSize(iSize, '\x0');

  //a_Do numeric multiplication
  register int iR;
  int iI, iJ, iK;
  for (iI = strXA.getSize() - 1; iI >= 0; iI--)
  {
    for (iJ = strXB.getSize() - 1; iJ >= 0; iJ--)
    {
      //a_Multiply
      iR = ((strXB.peek(iJ) - '0') * (strXA.peek(iI) - '0'));
      strR.use(iI + iJ + 1) += iR;
      
      //a_Adjust for overflow
      for (iK = iI + iJ + 1; iK > 0 ; iK--)
      {
        while(strR.peek(iK) >= '\xA')
        {
          //a_Carry to the next left digit
          strR.use(iK - 1)++;
          strR.use(iK) -= '\xA';
        }
      }
    }
  }

  //a_Offset base to '0'
  for (iI = 0x0; iI < iSize; iI++) strR.use(iI) += '0';

  //a_Removes leading '0'
  strR.stripLeading(ANumber::sstr_NumberWhiteSpace);

  //a_Now insert the decimal point
  if (decimal_a != AConstant::npos || decimal_b != AConstant::npos)
  {
    //a_If decimal doesn't exist, it shouldn't affect the decimal placement
    int p = a1;
		p = - p - (int)b1;
		_strPow10(strR, p);
  }

  //a_Cleanup
  _cleanString(strR);
  _fixPrecision(strR);

  return;
}

void ANumber::_strDivide(const AString& strA, const AString& strB, AString& strR, int iDesiredPrecision)
{
  AASSERT(this, !ANumber(strB).isZero());         //a_Just in case
  
  u4 decimal_a = strA.find('.'),
     decimal_b = strB.find('.');

  //a_Special case to accelerate the division process A/A == 1
  if (_strCompare(strA, strB) == 0x0)
  {
    strR = AConstant::ASTRING_ONE;
    return;
  }
  
  //a_Division by 1
  if (strB == AConstant::ASTRING_ONE)
  {
     strR = strA;
     return;
  }
  
  //a_Resize and clean the string
  strR.clear();

  //a_Generate initial number to start division
  char cR;
  AString strWork;
  AString strTemp, strXA(strA), strXB(strB);
  bool boolDoNotRound = false,      //a_If rounding is need (true if result found before precision reached)
       boolZeroAdded  = false;     //a_Cary of more that 1 zero results in a zero into R

  strXA.stripLeading(ANumber::sstr_NumberWhiteSpace);

  //a_Shift the divisor and divident(?)
  u4 shift_a = 0, shift_b = 0;
  if (decimal_b != AConstant::npos)
  {  
		shift_b = strB.getSize() - decimal_b - 1;
		if (shift_b)
      _strPow10(strXB, shift_b);
  }

  if (decimal_a != AConstant::npos)
	{
		shift_a =  strA.getSize() - decimal_a - 1;
    if (shift_a)
      _strPow10(strXA, shift_a);
	}

  int iShift      = shift_b - shift_a;

  //a_Start the divident (this is what I am calling it!)
  strWork = strXA.peek(0x0);
  u4 uA = 0x1;

  int iCarried = 0;
  while (iDesiredPrecision >= (iCarried - (int)shift_b))
  {
    while(_strCompare(strWork, strXB) < 0)
    {
      if (uA < strXA.getSize())
      {
        if (strWork == AConstant::ASTRING_ZERO)  
        {
          if (strXA.peek(uA) == '0')
          {
            //a_So far divided evenly, zeros are carried over
            strR += '0';
            uA++;
          }
          else
          {
            //a_Start again after even division
            strWork = strXA.peek(uA);
            uA++;

            //a_More that one digit added
            if (boolZeroAdded == true)
            {
              strR += '0';
            }
            boolZeroAdded = true;
          }
        }
        else
        {
          strWork += strXA.peek(uA);
          uA++;

          //a_More that one digit added
          if (boolZeroAdded == true)
          {
            strR += '0';
          }
          boolZeroAdded = true;
        }
      }
      else
      {
        if (strWork == AConstant::ASTRING_ZERO) break;  //a_We are done

        iCarried++;

        if (boolZeroAdded == true)
        {
          strR += '0';
        }
        strWork += '0';
        iShift--;
        boolZeroAdded = true;
      }
    }
  
    //a_Pre division check
    if (strWork == AConstant::ASTRING_ZERO && uA >= strXA.getSize())
    {
      boolDoNotRound = true;
      break;     //a_Found result
    }

    //a_Now do the subtraction
    boolZeroAdded = false;        //a_Reset the carry
    cR = '0';
    int iResult;
    while ((iResult = _strCompare(strWork, strXB)) >= 0)
    {
      strTemp = strWork;
      _strSubtract(strWork, strXB, strTemp);
      strWork = strTemp;
      cR++;
    }
    strR += cR;

    //a_Post division check
    if (strWork == AConstant::ASTRING_ZERO && uA >= strXA.getSize())
    {
      boolDoNotRound = true;
      break;     //a_Found result
    }
  }

  strR.stripTrailing(AConstant::ASTRING_SPACE);

  //a_Undo the shift
  if (iShift)
    _strPow10(strR, iShift);

  //a_Clean the result
  _cleanString(strR);
  _fixPrecision(strR);
  
  return;
}

void ANumber::_strPower(const AString& strA, const AString& strB, AString& strR)
{  
  strR = strA;
  AString strX, strXB(strB);
  _strSubtract(strXB, AConstant::ASTRING_ONE, strXB);             //a_strR=strA is one order already
  while (strXB != AConstant::ASTRING_ZERO)
  {
    _strSubtract(strXB, AConstant::ASTRING_ONE, strXB);
    strX = strR;
    _strMultiply(strA, strX, strR);
  }

  //a_Cleanup
  _cleanString(strR);
  _fixPrecision(strR);
}


//a_   |  A0  | A1 |
//a_   |000000.0000|
//a_          ^   
//a_      DecimalA
//a_MUST RETURN: -1 for less that, 0 for equals, 1 for greater that
int ANumber::_strCompare(const AString& strA, const AString& strB) const
{
  //a_Get the properties of the 2 strings
  u4 decimal_a = strA.find('.'),
     decimal_b = strB.find('.'),
      a0  = (decimal_a == AConstant::npos ? strA.getSize() : decimal_a),
      b0  = (decimal_b == AConstant::npos ? strB.getSize() : decimal_b);

  //a_A>B
  if (a0 > b0)
    return 1;

  if (b0 > a0)
    return -1;

  u4 uA = 0x0, uB = 0x0;

 //a_When decimals are equal, we have to test the number
  while ((uA < strA.getSize()) && (uB < strB.getSize()))
  {
    if (strA.peek(uA) > strB.peek(uB))
      return 1;
    if (strB.peek(uB) > strA.peek(uA))
      return -1;
  
    //a_Digits are equal
    uA++;
    uB++;
    
    //a_Ended together
    if ((uA >= strA.getSize()) && (uB >= strB.getSize()))
      return 0;
    //a_A ran out of digits, B wins
    if (uA >= strA.getSize()) return -1;
    //a_B ran out of digits, A wins
    if (uB >= strB.getSize()) return 1;
  }

  return 0;     //a_After all the tests, they are indeed equal
}



void ANumber::_strPow10(AString& strA, int iShift)
{
  u4 decimal = strA.find('.');
  if (decimal == AConstant::npos && iShift < 0)
  {
    strA += '.';
    decimal = strA.getSize() - 1;
  }

  //a_We have the decimal point
  if (iShift > 0)
  {
    if (decimal != AConstant::npos)
    {
      while (((decimal + 1) < strA.getSize()) && iShift > 0)
      {
        strA.use(decimal) = strA.peek(decimal + 1);
        iShift--; 
        decimal++;
      }
      if (iShift > 0)
      {
        strA.use(decimal) = '0';     //a_Last allocated digit
        iShift--;
      }
      else
      {
        strA.use(decimal) = '.';            //a_Put back the decimal point
      }
    }

    while (iShift > 0)
    {
      strA += '0';
      iShift--;
    }
  }
  else if (iShift < 0)
  {
    while (iShift < 0 && decimal > 0)
    {
      strA.use(decimal) = strA.peek(decimal - 1);
      iShift++;
      decimal--;
    }

    //a_First character (special case)
    if (iShift < 0)
    {
      strA.use(0x0) = '.';
    }
    else if (iShift == 0)
    {
      //a_Sift within character
      strA.use(decimal) = '.';
    }

    //a_Need insertion
    if (iShift < 0)
    {
      //a_Insert zero time
      AString strTemp;
      if (iShift < -1)
      {
        strTemp.setSize(-iShift - 1, '0');
      }
      strA.use(0x0) = '0';
      strA = AString("0.") + strTemp + strA;
    }
  
    if (strA.peek(0) == '.')
    {
      strA = AString('0') + strA;
    }
  }

  //a_Cleanup
  _cleanString(strA);
  _fixPrecision(strA);
}

void ANumber::toAFile(AFile &aFile) const
{
  //a_Save members
  aFile.write(mc__Sign);
  mstr__Number.toAFile(aFile);
  aFile.write(m__Precision);
}

void ANumber::fromAFile(AFile &aFile)
{
  //a_Restore members
  aFile.read(mc__Sign);
  mstr__Number.fromAFile(aFile);
  aFile.read(m__Precision);
}

ANumber ANumber::factorial(const ANumber &nSource)
{
  if (nSource < ANumber(0x0))
    ATHROW(NULL, AException::NegativeFactorial);
  
  if(nSource.isWhole() == false)
    ATHROW(NULL, AException::NonWholeFactorial);

  ANumber nR(1), nX(nSource);
  while (nX > ANumber(0x1))
  {
    nR *= nX;
    nX--;
  }

  return nR;
}

ANumber ANumber::e(const ANumber &nSource, int iPrecision)
{
  if (iPrecision < 0) iPrecision = DBL_DIG;
  iPrecision += 3;    //a_Rounding buffer

  ANumber nR(1);          //a_Term 0 is always 1
  nR.setPrecision(iPrecision);
  
  //a_Worker values
  ANumber nTerm, nT, nX(nSource);
  nT.setPrecision(iPrecision);
  nX.setPrecision(iPrecision);
  
  ANumber nF, nP;
  nF.setPrecision(iPrecision);
  nP.setPrecision(iPrecision);
  for (nT = 1 ;; nT++)
  {
    nP = nX.power(nT);

    nF = ANumber::factorial(nT);

    nTerm = nP / nF;
    nTerm.setPrecision(iPrecision);

    //a_Determine if you are withing precision (since this series converges)
    if (nTerm.isZero()) break;

    nR += nTerm;
  }

  //a_Reset precision (if it is less that standard DLB_DIG)
  iPrecision -= 3;    //a_Rounding buffer
  nR.setPrecision(iPrecision);

  return nR;
}

ANumber ANumber::sin(const ANumber &nSource, int iPrecision)
{
  if (iPrecision < 0) iPrecision = DBL_DIG;

  iPrecision += 3;    //a_Rounding buffer

  ANumber nR(nSource);                  //a_Term 0 is always nSource in Sin series
  nR.setPrecision(iPrecision);

  //a_Adjust all precisions (in case they are less than standard)
  ANumber nToggle(-0x1), nTerm, nT(0x3);
  nT.setPrecision(iPrecision);
  nToggle.setPrecision(iPrecision);

  for (;; nT += ANumber(0x2))
  {
    ANumber nX(nSource);
    nX.setPrecision(iPrecision);
  
    nTerm = nX.power(nT) / ANumber::factorial(nT);
    nTerm.setPrecision(iPrecision);

    //a_Determine if you are withing precision (since this series converges)
    if (nTerm.isZero()) break;

    nR += nToggle * nTerm;
    
    nToggle.toggleSign();  //a_Fast sign inverter
  }

  //a_Re-adjust precision in case it is less that DBL_DIG
  iPrecision -= 3;    //a_Rounding buffer
  nR.setPrecision(iPrecision);

  return nR;
}

ANumber ANumber::cos(const ANumber &nSource, int iPrecision)
{
  if (iPrecision < 0) iPrecision = DBL_DIG;

  iPrecision += 3;    //a_Rounding buffer

  ANumber nR(1);                  //a_Term 0 is always 1 in Cos series
  nR.setPrecision(iPrecision);

  //a_Adjust all precisions (in case they are less than standard)
  ANumber nToggle(-0x1), nTerm, nT(0x2);
  nT.setPrecision(iPrecision);
  nToggle.setPrecision(iPrecision);

  for (;; nT += ANumber(0x2))
  {
    ANumber nX(nSource);
    nX.setPrecision(iPrecision);
    nTerm = nX.power(nT) / ANumber::factorial(nT);
    nTerm.setPrecision(iPrecision);

    //a_Determine if you are withing precision (since this series converges)
    if (nTerm.isZero()) break;

    nR += nToggle * nTerm;
    
    nToggle.toggleSign();  //a_Fast sign inverter
  }

  //a_Re-adjust precision in case it is less that DBL_DIG
  iPrecision -= 3;    //a_Rounding buffer
  nR.setPrecision(iPrecision);

  return nR;
}

ANumber& ANumber::operator=(double source)
{
  // -aaaa.bbbbb or -aaa.bbbe+ccc
#if (_MSC_VER >= 1400)
  char pcBuffer[_CVTBUFSIZE];
  errno_t errornum = _gcvt_s(pcBuffer, _CVTBUFSIZE, source, _CVTBUFSIZE-16);
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
  setNumber(pcBuffer);
#else
  char pcBuffer[_CVTBUFSIZE];
  setNumber(gcvt(source, _CVTBUFSIZE-16, pcBuffer));
#endif;
  
  return *this;
}
