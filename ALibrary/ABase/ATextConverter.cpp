//
// ATextConverter
//

#include "pchABase.hpp"

#include "ATextConverter.hpp"
#include "AException.hpp"
#include <ctype.h>

void ATextConverter::encodeBase64(const AString& strSource, AOutputBuffer& target)
{
  int iOctet, iTemp = 0, iBits = 0;
  for (size_t i=0; i<strSource.getSize(); ++i)
  {
    iOctet = strSource[i];
    iTemp <<= 8;
    iTemp |= 0xff & iOctet;
    iBits += 8;
    while( iBits >= 6 )
    {
      iBits -= 6;
      target.append(AConstant::CHARSET_BASE64[ 0x3f & (iTemp >> iBits) ]);
    }
  }
	
	if( iBits > 0 )
  {
    iTemp <<= 6 - iBits;
    target.append(AConstant::CHARSET_BASE64[ 0x3f & iTemp ]);
    if ( iBits == 4 )
      target.append('=');
    else if ( iBits == 2 )
      target.append("==", 2);
  }
}

void ATextConverter::decodeBase64(const AString& strSource, AOutputBuffer& target)
{
  decodeBase64(strSource, 0, target);
}

void ATextConverter::decodeBase64(const AString& strSource, size_t startPos, AOutputBuffer& target)
{
  bool boolFlushing = false;
  int iTemp = 0, iBits = 0, iCharacter;
  for (size_t i=startPos; i<strSource.getSize(); ++i)
  {
    iCharacter = strSource[i];
    if (!isspace(iCharacter))
    {
      if (boolFlushing)
      {
        if (iCharacter != '=')
        {
          //a_Invalid Base 64 AString
          ATHROW_EX(NULL, AException::InputSourceInvalid, ASWNL("One of terminating '=' is missing"));
          break;
        }
      }
      else 
      {
        if( iCharacter == '=' )
          boolFlushing = true;
        else
        {
          iTemp <<= 6;
          iTemp |= _decodeBase64Character(iCharacter);
          iBits += 6;
          if( iBits >= 8 )
          {
            iBits -= 8;
            target.append((0xff & (iTemp >> iBits)));
          }
        }
      }
    }
  }
}

int ATextConverter::_decodeBase64Character(int iCharacter)
{
  int iSextet = -1;     //a_Invalid by default

  if ( ('A' <= iCharacter) && (iCharacter <= 'Z') )      iSextet = iCharacter - 'A';
  else if ( ('a' <= iCharacter) && (iCharacter <= 'z') ) iSextet = (iCharacter - 'a') + 26;
  else if ( ('0' <= iCharacter) && (iCharacter <= '9') ) iSextet = (iCharacter - '0') + 52;
  else if (iCharacter == '+')                            iSextet = 62;
  else if (iCharacter == '/')                            iSextet = 63;

  return iSextet;
}

void ATextConverter::encode64(
  const AString& strSource, 
  AOutputBuffer& target, 
  const AString& strCharSet // = sm__strAlphaNumCharacterSet
)
{
  AASSERT(NULL, strCharSet.getSize() == 64);  //a_Must always be 64 characters

  int iOctet, iTemp = 0, iBits = 0;
  for (u4 i=0; i<strSource.getSize(); ++i)
  {
    iOctet = strSource[i];
    iTemp <<= 8;
    iTemp |= 0xff & iOctet;
    iBits += 8;
    while( iBits >= 6 )
    {
      iBits -= 6;
			target.append(strCharSet[ 0x3f & (iTemp >> iBits) ]);
    }
  }
	
	if( iBits > 0 )
  {
    iTemp <<= 6 - iBits;
    target.append(strCharSet[ 0x3f & iTemp ]);
    if ( iBits == 4 )
      target.append('.');
    else if ( iBits == 2 )
    {
      target.append("..", 2);
    }
  }
}

void ATextConverter::decode64(
  const AString& strSource, 
  AOutputBuffer& target,
  const AString& strCharSet // = sm__strAlphaNumCharacterSet
)
{
  AASSERT(NULL, strCharSet.getSize() == 64);  //a_Must always be 64 characters

  bool boolFlushing = false;
  int iTemp = 0, iBits = 0, iCharacter;
  for (u4 i=0; i<strSource.getSize(); ++i)
  {
    iCharacter = strSource[i];
    if (!isspace(iCharacter))
    {
      if (boolFlushing)
      {
        if (iCharacter != '.')
        {
          //a_Invalid Base 64 AString
          ATHROW_EX(NULL, AException::InputSourceInvalid, ASWNL("One of terminating '.' is missing"));
          break;
        }
      }
      else 
      {
        if( iCharacter == '.' )
          boolFlushing = true;
        else
        {
          size_t x = strCharSet.find(iCharacter);
          if (x == AConstant::npos)
            ATHROW(NULL, AException::InvalidEncoding);
          
          iTemp <<= 6;
          iTemp |= (u1)x;
          iBits += 6;
          if( iBits >= 8 )
          {
            iBits -= 8;
            target.append((0xff & (iTemp >> iBits)));
          }
        }
      }
    }
  }
}

void ATextConverter::encodeHEX(const AString& strSource, AOutputBuffer& target)
{
  for (u4 i = 0; i<strSource.getSize(); ++i)
    convertBYTEtoHEX(strSource[i], target);
}

void ATextConverter::decodeHEX(const AString& strSource, AOutputBuffer& target)
{
  size_t len = strSource.getSize();
  
  //a_MUST be even (ignore last char if not even)
  if (len != ((len >> 0x1) << 0x1))
    len -= len-1;
  
  for (size_t i=0; i<len; i+=2)
    target.append(convertHEXtoBYTE(strSource[i], strSource[i+1]));
}

//a_Converts 2 digit hex to a value 0-0xFF
u1 ATextConverter::convertHEXtoBYTE(char cHi, char cLo)
{
  if (isxdigit((u1)cHi) && isxdigit((u1)cLo))
  {
    if (isdigit((u1)cHi)) cHi -= '0';
    else cHi = tolower(cHi) - 'a' + '\xA';

    if (isdigit((u1)cLo)) cLo -= '0';
    else cLo = tolower(cLo) - 'a' + '\xA';

    return cHi * '\x10' + cLo; 
  }

  ATHROW_EX(NULL, AException::InvalidParameter, ARope("cHi=")+AString::fromInt(cHi)+" cLo="+AString::fromInt(cLo));
}

//a_Converts a value 0-0xFF to a 2 digit hex AString
AString ATextConverter::convertBYTEtoHEX(u1 byteV)
{
  AString strResult;

  //a_Do the convert, base 16 a BYTE will always be 2 characters wide...
  char cT = char(byteV / 0x10),
       cR = char(byteV % 0x10);

  cT += (cT > '\x9' ? ('a' - '\xA') : '0');
  cR += (cR > '\x9' ? ('a' - '\xA') : '0');

  strResult  = cT;
  strResult += cR;

  return strResult;
}

void ATextConverter::convertBYTEtoHEX(u1 byteV, AOutputBuffer& target)
{
  AString strResult;

  //a_Do the convert, base 16 a BYTE will always be 2 characters wide...
  char cT = char(byteV / 0x10),
       cR = char(byteV % 0x10);

  cT += (cT > '\x9' ? ('a' - '\xA') : '0');
  cR += (cR > '\x9' ? ('a' - '\xA') : '0');

  target.append(cT);
  target.append(cR);
}

void ATextConverter::encodeURL(
  const AString& strSource, 
  AOutputBuffer& target,
  bool isUrlPerRFC2396 // = false
)
{
  unsigned char cX;
  for (u4 i = 0x0; i <strSource.getSize(); ++i)
  {
    cX = strSource[i];
    
    if (isUrlPerRFC2396)
    {
      //a_Determine if to encode or not
      if (' ' == cX)
      {
        target.append('+');     //a_Space encodes to a plus
      }
      else if (AConstant::npos == AConstant::CHARSET_URL_RFC2396.find(cX))
      {
        //a_Convert to hex
			  target.append('%');
        convertBYTEtoHEX((u1)cX, target);
      }
		  else
      {
        target.append(cX);
		  }
    }
    else
    {
      //a_Encode all non-alphanum
      if (isalnum(cX))
      {
        target.append(cX);
      }
      else if (' ' == cX)
      {
        target.append('+');     //a_Space encodes to a plus
      }
      else
      {
        //a_Convert to hex
			  target.append('%');
        convertBYTEtoHEX((u1)cX, target);
      }
    }
  }
}

void ATextConverter::decodeURL(
  const AString& strSource, 
  AOutputBuffer& target,
  bool boolIgnoreNulls,   // = true
  bool boolForceUppercase // = false
)
{
  char cX;
  for (u4 i = 0x0; i <strSource.getSize(); ++i)
  {
    cX = strSource[i];
    switch (cX)
    {
      case '+' : //a_Replace with space
        target.append(' ');
      break;

      case '%' : //a_URL escape code
        //a_Convert if we have space, hate to get an access exception
        //a_ If this is invalid, ignore the %
        if ((i + 2) < strSource.getSize())
        {
          cX = convertHEXtoBYTE(strSource[i + 0x1], strSource[i + 0x2]);
          i += 2;

          if (cX == '\x0' && boolIgnoreNulls == true)
            continue;

          //a_See if we need to force uppercase
          if (boolForceUppercase)
            target.append(toupper(cX));
          else
            target.append(cX);
        }
      break;

      default :
        //a_See if we need to force uppercase
        if (boolForceUppercase)
          target.append(toupper(cX));
        else
          target.append(cX);
    }
  }
}

void ATextConverter::makeHtmlSafe(const AString& strSource, AOutputBuffer& target)
{
  static const AString unsafeHtml("<>&");

  size_t lastPos = 0, newPos = 0;
  if ( AConstant::npos == (newPos = strSource.findOneOf(unsafeHtml, lastPos)) )
  {
    //a_Nothing to do
    strSource.emit(target);
    return;
  }
  
  AString strPeek;
  do
  {
    if (newPos > lastPos)
    {
      //a_Something to copy, when == then special chars next to each other
      strPeek.clear();
      strSource.peek(strPeek, lastPos, newPos - lastPos);
      target.append(strPeek);
    }
    
    switch(strSource.at(newPos))
    {
      case '<' : target.append("&lt;"); break;
      case '>' : target.append("&gt;"); break;
      case '&' : target.append("&amp;"); break;
      default : AASSERT(NULL, 0x0);
    }

    lastPos = newPos + 1;
  }
  while (AConstant::npos != (newPos = strSource.findOneOf(unsafeHtml, lastPos)));

  //a_Leftovers
  strPeek.clear();
  strSource.peek(strPeek, lastPos);
  target.append(strPeek);
}

void ATextConverter::makeCDataSafe(const AString& strSource, AOutputBuffer& target)
{
  static const AString unsafeCData("]]>");          //a_Cannot have this equence in CDATA
  static const AString safeCData("%5d%5d%3e");      //a_Replacement
  
  size_t lastPos = 0, newPos = 0;
  if (AConstant::npos == (newPos = strSource.find(unsafeCData, lastPos)))
  {
    strSource.emit(target);  //a_Nothing to do
    return;
  }

  AString strPeek;
  do
  {
    if (newPos > lastPos)
    {
      //a_Something to copy, when == then special chars next to each other
      strPeek.clear();
      strSource.peek(strPeek, lastPos, newPos - lastPos);
      target.append(strPeek);
    }
    
    target.append(safeCData);

    lastPos = newPos + 0x3;
  }
  while (AConstant::npos != (newPos = strSource.find(unsafeCData, lastPos)));

  //a_Leftovers
  strPeek.clear();
  strSource.peek(strPeek, lastPos);
  target.append(strPeek);
}


void ATextConverter::convertStringToHexDump(const AString& source, AOutputBuffer& target, bool boolIncludeNonAscii)
{
  AString strTemp;
  static const AString strZeros("00000000");    //a_Maximum of 8 used in padding
  size_t size = source.getSize();
  size_t rows = size >> 0x4;           //a_16 items per row
  char cX;
  size_t i;

  //a_Length in hex (dec)
  target.append("Length=0x",9);
  target.append(AString::fromSize_t(size, 0x10));
  target.append(" (", 2);
  target.append(AString::fromSize_t(size, 0xA));
  target.append(')');
  target.append(AConstant::ASTRING_CRLF);
  
  //a_Complete rows
  size_t y;
  for (y = 0; y < rows; ++y)
  {
    //a_Skip a line every 8
    if (!(y % 0x8))
      target.append(AConstant::ASTRING_CRLF);

    //a_Get the offset
    strTemp = AString::fromSize_t(y * 0x10, 0x10);
    strTemp.insert(0, strZeros, 0, 8 - strTemp.getSize());
    target.append(strTemp);
    
    //a_Now the ASCII
    target.append(": ");
    strTemp.clear();
    source.peek(strTemp, y * 0x10, 0x10);
    for (i = 0; i < 0x10; ++i)
    {  
      cX = strTemp.at(i);
      if (!boolIncludeNonAscii && !isalnum((u1)cX))
        target.append('.');
      else
        target.append(cX);
    }
    
    target.append(" - ",3);

    //a_Now the hex values
    for (i = 0; i < 0x10; ++i)
    {
      target.append(' ');
      target.append(convertBYTEtoHEX(source.at(y * 0x10 + i)));
    }

    target.append(AConstant::ASTRING_CRLF);
  }
  
  //a_Remaineder
  assert(source.getSize() >= y * 0x10);
  size_t remainder = source.getSize() - y * 0x10;
  if (remainder)
  {
    //a_Get the offset
    strTemp = AString::fromS4((s4)(y * 0x10), 0x10);
    strTemp.insert(0, strZeros, 0, 8 - strTemp.getSize());
    target.append(strTemp);

    //a_Now the ASCII
    target.append(": ",2);
    strTemp.clear();
    source.peek(strTemp, y * 0x10, remainder);
    for (i = 0; i < remainder; ++i)
    {  
      cX = strTemp.at(i);
      if (isalnum((u1)cX))
        target.append(cX);
      else
        target.append('.');
    }

    //a_Pad
    for (i; i < 0x10; ++i)
      target.append(' ');

    target.append(" - ",3);

    //a_Now the hex values
    for (i = 0; i < remainder; ++i)
    {
      target.append(' ');
      target.append(convertBYTEtoHEX(source.at(y * 0x10 + i)));
    }

    target.append(AConstant::ASTRING_CRLF);
  }
}

void ATextConverter::makeAsciiPrintable(const AString& strSource, AOutputBuffer& target)
{
  AString strPeek;
  char c = 0;
  for(size_t i=0; i<strSource.getSize(); ++i)
  {
    c = strSource.at(i);
    if (isalnum((u1)c))
    {
      target.append(c);
    }
    else
    {
      target.append("\\x",2);
      convertBYTEtoHEX(c, target);
    }
  }
}

