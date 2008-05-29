/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ansiHelpers.hpp"

#ifdef NEEDS_ANSI_HELPERS
  char *strrev (char *pcString)
  {
    char *pcStart = pcString, *pcLeft = pcString;

    while (*pcString++);                 //a_Find end of string
 
    pcString -= 2;

    //a_Do reversal
    char cX;
    while (pcLeft < pcString)
    {
      cX = *pcLeft;
      *pcLeft++ = *pcString;
      *pcString-- = cX;
    }

    return (pcStart);
  }

  int strnicmp(const char *pccOne, const char *pccTwo, u4 uLength)
  {
    int i1, i2;
    if ( uLength )
    {
      do
      {
        if ( ((i1 = (u1)(*(pccOne++))) >= 'A') && (i1 <= 'Z') )
          i1 -= 'A' - 'a';

        if ( ((i2 = (u1)(*(pccTwo++))) >= 'A') && (i2 <= 'Z') )
          i2 -= 'A' - 'a';
      }
      while ( --uLength && i1 && (i1 == i2) );

      return (i1 - i2);
    }

    return 0x0;
  }

  //
  // Conversion routines numeric type to string
  //
  void xtoa (u4 uNumber, char *pcBuffer, u4 uRadix, int iIsNegative)
  {
    u4 uDigitValue;          //a_Digito, valuo, weuso
    char *pcX = pcBuffer,    //a_Temp pointer to the string so we can traverse it
         *pcFirstDigit,      //a_Pointer to the first digit
          cTemp;

    //a_Negative, we need a minus '-' and negate the number
    if (iIsNegative)
    {                
      *pcX++ = '-';
      uNumber = (u4)(-(long)uNumber);
    }

    pcFirstDigit = pcX;           //a_Gotta know where the first digit is

    do
    {
      uDigitValue = (unsigned) (uNumber % uRadix);   //a_Value of the digit in light of radix
      uNumber /= uRadix;                             //a_Is next, svimvear!

      //a_Convert to ASCII
      if (uDigitValue > 9)
        *pcX++ = (char)(uDigitValue - 10 + 'a');      //a_We have an alpha
      else
        *pcX++ = (char)(uDigitValue + '0');           //a_We have a digit

    } while (uNumber > 0);

    //a_Reverse to follow
    *pcX-- = '\0';            //a_NULL terminate since pcX points to last digit in our string

    do
    {
      cTemp = *pcX;
      *pcX = *pcFirstDigit;
      *pcFirstDigit = cTemp;       //a_Prestochangodigito
      --pcX;
      ++pcFirstDigit;             //a_More, more, more...

    } while (pcFirstDigit < pcX); //a_Until we get to the middle, then stop...
  }

  // Actual functions just call conversion helper with neg flag set correctly, and return pointer to buffer. */
  char *itoa (int iNumber, char *pcBuffer, u4 uRadix)
  {
    if (uRadix == 10 && iNumber < 0)
      xtoa((u4)iNumber, pcBuffer, uRadix, 1);
    else
      xtoa((u4)(u4)iNumber, pcBuffer, uRadix, 0);

    return pcBuffer;
  }

  char *ltoa (long lNumber, char *pcBuffer, u4 uRadix)
  {
    xtoa((u4)lNumber, pcBuffer, uRadix, (uRadix == 10 && lNumber < 0));
    return pcBuffer;
  }

  char *ultoa (u4 uNumber, char *pcBuffer, u4 uRadix)
  {
    xtoa(uNumber, pcBuffer, uRadix, 0);
    return pcBuffer;
  }
#endif