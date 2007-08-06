#include "pchABase.hpp"
#include "ATextGenerator.hpp"
#include "ATime.hpp"
#include "ATextConverter.hpp"
#include "AException.hpp"
#include "AString.hpp"

int ATextGenerator::si_Counter(0x0);

//a_Complely random number from sm__strNumber
void ATextGenerator::generateRandomNumber(AOutputBuffer& target, size_t len /* = 0x1 */)
{
  for (u4 i = 0x0; i < len; i++)
    target.append(generateRandomNumeral());
}

//a_Complely random number from sm__strNumber
void ATextGenerator::generateRandomAlphanum(AOutputBuffer& target, size_t len /* = 0x1 */)
{
  generateRandomString(target, len, AString::sstr_AlphaNum);
}

void ATextGenerator::generateFromTemplate(AOutputBuffer& target, const AString &strTemplate)
{
  if (strTemplate.isEmpty())
    return;
  
  //a_If the last is % and one before is not %, then expansion error will occur
  size_t end = strTemplate.getSize();
  if (strTemplate[end - 1] == '%')
  {
    if (end < 1)
    {
      //a_Force early termination
      end = 0;
    }
    else
      if (strTemplate[end - 2] != '%')
      {
        end--;
      }
  }
  
  char cEscape;
  for (size_t x = 0; x < end; ++x)
  {
    cEscape = strTemplate[x];
    if (cEscape == '%')
    {
      if (++x >= end)
        break;

      switch(strTemplate[x])
      {
        case '%' : break;
        
        case 'z' :
          target.append(AString::fromInt(si_Counter++));
          cEscape = '\x0';
        break;

        case 'n' :
          cEscape = generateRandomNumeral();
        break;

        case 'N' :
          if (ARandomNumberGenerator::get().nextRange(100) >= 50)
            cEscape = '+';
          else
            cEscape = '-';
        break;

        case 'l' :
          cEscape = generateRandomLowercaseLetter();
        break;

        case 'L' :
          cEscape = generateRandomUppercaseLetter();
        break;

        case 'r' :
          cEscape = ARandomNumberGenerator::get().nextRange(255) + 0x1;
        break;

        case 'R' :
          cEscape = ARandomNumberGenerator::get().nextU1();
        break;

        case 's' :
          cEscape = generateRandomUppercaseLetter();
          if (ARandomNumberGenerator::get().nextRange(100) >= 50)
            cEscape = tolower(cEscape);
        break;
      
        case 't' :
          ATime().emitYYYYMMDDHHMMSS(target);
          cEscape = '\x0';
        break;
        
        case 'T' :
          ATime().emitRFCtime(target);
          cEscape = '\x0';
        break;

        default :
          //a_Escape character detected, but no valid control character found
          target.append(cEscape);          //a_Keep the sequence untouched
          cEscape = strTemplate[x];  //a_Current char becomes escape to stay consistent with the add to follow
      }
    }

    if (cEscape != '\x0')
      target.append(cEscape);
  }
}

void ATextGenerator::generateRandomString(AOutputBuffer& target, size_t len, const AString& strCharSet)
{
  for (size_t i = 0x0; i < len; ++i)
  {
    target.append(strCharSet.peek(ARandomNumberGenerator::get().nextRange(strCharSet.getSize())));
  }
}

void ATextGenerator::generateRandomData(AOutputBuffer& target, size_t len)
{
  for (size_t i = 0x0; i < len; ++i)
  {
    target.append(ARandomNumberGenerator::get().nextU1());
  }
}

void ATextGenerator::generateUniqueId(AOutputBuffer& target, size_t size /* = 32 */)
{
  if (size < 16)
    ATHROW(NULL, AException::InvalidParameter);

  ARope rope;
  size_t x = ATime::getTickCount();
  rope.append((const char *)&x, sizeof(size_t));
  
  size_t bytesToAdd = size / 2 - sizeof(size_t);
  while(bytesToAdd >= 4)
  {
    x = ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer).nextU4();
    rope.append((const char *)&x, 4);
    bytesToAdd -= 4;
  }

  ATextConverter::encode64(rope, target);
  if (size > rope.getSize())
    ATextGenerator::generateRandomString(target, size - rope.getSize());
}

void ATextGenerator::generateRandomWord(AOutputBuffer& target, size_t len /* = 0x1 */)
{
  if (len < 1)
    return;

  //a_Lead with consonant
  --len;
  AString str, strLast("-", 1);
  generateRandomString(str, 1, AString::sstr_LowerCaseConsonants);
  str.makeUpper();
  target.append(str);

  bool boolConsonant = false;
  while (len > 0)
  {
    if (boolConsonant)
    {
      //a_Add a consonant    
      do
      {
        str.clear();
        generateRandomString(str, 1, AString::sstr_LowerCaseConsonants);
      }
      while(str.at(0) == strLast.at(0) && ARandomNumberGenerator::get().nextU1() > 220);

      target.append(str);
      --len;
      strLast = str;
      boolConsonant = false;
    }
    else
    {
      //a_Add a vowel
      int twice = 0;
      do
      {
        do
        {
          str.clear();
          generateRandomString(str, 1, AString::sstr_LowerCaseVowels);
        }
        while(str.at(0) == strLast.at(0) && ARandomNumberGenerator::get().nextU1() > 130);

        target.append(str);
        --len;
        strLast = str;
      }
      while (len > 0 && twice++ < 2 && ARandomNumberGenerator::get().nextU1() > 225);
  
      boolConsonant = true;
    }
  }
}

