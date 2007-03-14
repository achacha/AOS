#include <iostream.h>
#include <preABase.hpp>

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

int main()
{
  char pcBuffer[128];
  xtoa (1024, pcBuffer, 0x10, 0);

  cerr << pcBuffer << " should be 0x400" << endl;

  return 0x0;
}
