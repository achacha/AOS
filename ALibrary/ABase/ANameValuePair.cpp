/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ANameValuePair.hpp"
#include "ATextConverter.hpp"
#include "AException.hpp"
#include "AFile.hpp"
#include "AOutputBuffer.hpp"
#include "ATextConverter.hpp"
#include <deque>
#include <algorithm>

void ANameValuePair::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  //a_Members
  ADebugDumpable::indent(os, indent+1) << "m_name='" << m_name << "'";
    os << "  m_value='" << m_value << "'" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_nvtype=" << m_nvtype << "  ";
    os << "  m_valueWrap=" << m_valueWrap << std::endl;

  AString str;
  ATextConverter::encodeURL(m_separator, str, true);
  ADebugDumpable::indent(os, indent+1) << "  m_separator='" << str << "'";
  str.clear();
  ATextConverter::encodeURL(m_terminator, str, true);
  os << "  m_terminator='" << str << "'";
  str.clear();
  ATextConverter::encodeURL(m_whitespace, str, true);
  os << "  m_whitespace='" << str << "'" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ANameValuePair::ANameValuePair(const AString& name, const AString& strValue, NameValueType eType) :
  m_name(128, 128),
  m_value(128, 128),
  m_separator("=",1),
  m_terminator("&\r\n",3),
  m_valueWrap('\x0'),
  m_nvtype(ANameValuePair::CGI)
{
  AASSERT(this, !name.isEmpty());

  if (ANameValuePair::CGI != eType)
    setType(eType);

  m_name.assign(name); 
  m_value.assign(strValue);
}

ANameValuePair::ANameValuePair(NameValueType eType) :
  m_name(128, 128),
  m_value(128, 128),
  m_separator("=",1),
  m_terminator("&\r\n",3),
  m_valueWrap('\x0'),
  m_nvtype(ANameValuePair::CGI)
{
  if (ANameValuePair::CGI != eType)
    setType(eType);
}

ANameValuePair::~ANameValuePair()
{
}

void ANameValuePair::emit(AOutputBuffer& target) const
{
  switch (m_nvtype)
  {
    case ANameValuePair::XML  :
    case ANameValuePair::HTML :
    case ANameValuePair::JSON :
      target.append(m_name);
      if (!m_value.isEmpty())
      {
        if (m_valueWrap)
        {
          target.append(m_separator);
          target.append(m_valueWrap);
          target.append(m_value);
          target.append(m_valueWrap);
        }
        else
        {
          target.append(m_separator);
          target.append(m_value);
        }
      }
      else
      {
        //a_If no value emit ="" to be well formed
        if (m_valueWrap)
        {
          target.append(m_separator);
          target.append(m_valueWrap);
          target.append(m_valueWrap);
        }
        else
        {
          target.append(m_separator);
          target.append("\"\"", 2);
        }
      }
    break;

    case ANameValuePair::CGI :
    case ANameValuePair::CGI_CASE :
      ATextConverter::encodeURL(m_name, target);
      target.append(m_separator);
      if (!m_value.isEmpty())
      {
        ATextConverter::encodeURL(m_value, target);
      }
    break;

    case ANameValuePair::REGINI :
      //a_Name and value both wrapped by " (as required)
      target.append('\"');
      target.append(m_name);
      target.append('\"');
      target.append(m_separator);
      target.append('\"');
      target.append(m_value);
      target.append('\"');
    break;

    case ANameValuePair::COOKIE :
    case ANameValuePair::DATA :
      target.append(m_name);
      if (!m_value.isEmpty())
      {
        target.append(m_separator);
        target.append(m_value);
      }
    break;

    case ANameValuePair::FORM_MULTIPART :
      target.append(m_name);
      if (!m_value.isEmpty())
      {
        target.append(m_separator);
        target.append('\"');
        ATextConverter::encodeURL(m_value, target);
        target.append('\"');
      }
    break;

    case ANameValuePair::CUSTOM :
    case ANameValuePair::HTTP :
      target.append(m_name);
      if (!m_value.isEmpty())
      {
        target.append(m_separator);
        target.append(m_value);
      }
    break;

    default :
      ATHROW(this, AException::UnknownType);
  }
}

void ANameValuePair::toAFile(AFile &aFile) const
{
  //a_Save data
  char i = '\x0';
  switch(m_nvtype)
  {
    case ANameValuePair::XML: i = 'x'; break;
    case ANameValuePair::HTML: i = 'h'; break;
    case ANameValuePair::JSON: i = 'j'; break;
    case ANameValuePair::CGI: i = 'c'; break;
    case ANameValuePair::CGI_CASE: i = 'C'; break;
    case ANameValuePair::REGINI : i = 'r'; break;
    case ANameValuePair::HTTP : i = 't'; break;
    case ANameValuePair::COOKIE : i = 'o'; break;
    case ANameValuePair::DATA : i = 'd'; break;
    case ANameValuePair::CUSTOM : i = 'u'; break;
    case ANameValuePair::FORM_MULTIPART : i = 'f'; break;
    default:
      ATHROW(this, AException::UnknownType);
  }
  aFile.write(m_valueWrap);
  m_name.toAFile(aFile);
  m_value.toAFile(aFile);
}

void ANameValuePair::fromAFile(AFile &aFile)
{
  //a_Restore data
  char i;
  aFile.read(i);
  switch(i)
  {
    case 'x': setType(ANameValuePair::XML); break;
    case 'h': setType(ANameValuePair::HTML); break;
    case 'j': setType(ANameValuePair::JSON); break;
    case 'c': setType(ANameValuePair::CGI); break;
    case 'C': setType(ANameValuePair::CGI_CASE); break;
    case 'r': setType(ANameValuePair::REGINI); break;
    case 't': setType(ANameValuePair::HTTP); break;
    case 'o': setType(ANameValuePair::COOKIE); break;
    case 'd': setType(ANameValuePair::DATA); break;
    case 'f': setType(ANameValuePair::FORM_MULTIPART); break;
    case 'u': setType(ANameValuePair::CUSTOM); break;
    default:
      ATHROW(this, AException::UnknownType);
  }

  aFile.read(m_valueWrap);
  m_name.fromAFile(aFile);
  m_value.fromAFile(aFile);
}

int ANameValuePair::compare(const ANameValuePair& nvSource) const
{
  int iRet = m_name.compare(nvSource.m_name);
  if (iRet)
    return iRet;

  iRet = m_value.compare(nvSource.m_value);
	if (iRet)
    return iRet;

  return 0x0;
}

bool ANameValuePair::isName(const AString& str) const
{
  return m_name.equals(str);
}

bool ANameValuePair::isValue(const AString& str) const
{
  return m_value.equals(str);
}

bool ANameValuePair::isNameNoCase(const AString& str) const
{
  if (m_name.compareNoCase(str))
    return false;
  else
    return true;
}

const AString& ANameValuePair::getName()  const
{
  return m_name;
}

const AString& ANameValuePair::getValue() const
{ 
  return m_value;
}

void ANameValuePair::clear()
{  
  //a_Clear internal pair
  m_name.clear();
  m_value.clear();
  
  switch(m_nvtype)
  {
    case ANameValuePair::XML    :
    case ANameValuePair::HTML :
      m_valueWrap = '\"';
    break;

    default:
      m_valueWrap = '\x0';
    break;
  }
}

void ANameValuePair::setType(NameValueType eType)
{
  m_nvtype = eType;

  switch (eType)
  {
    case ANameValuePair::XML    :
    case ANameValuePair::HTML :
      //a_<TYPE NAME=VALUE NAME2='VALUE2' NAME3="some'value">
      setDelimeters("=", ">/");
      setWhiteSpace(" \r\n\t");
      m_valueWrap = '\"';
    break;

    case ANameValuePair::JSON :
      //a_NAME:VALUE or NAME2='VALUE2' or NAME3="some'value"
      setDelimeters(":", "\"\'");           
      setWhiteSpace("\" \r\n\t");
      m_valueWrap = '\x0';
    break;

    case ANameValuePair::CGI : 
    case ANameValuePair::CGI_CASE : 
      //a_NAME=VALUE&NAME2=VALUE2
      setDelimeters("=", "&\r\n");        
      setWhiteSpace("");
      m_valueWrap = '\x0';
    break;

    case ANameValuePair::REGINI :
      //a_REGINI profile type
      //a_"Name"="Value" (both quoted)
      setDelimeters("=", "\"");        
      setWhiteSpace(" \r\n\t");
      m_valueWrap = '\"';
    break;
    
    case ANameValuePair::HTTP :
      //a_NAME: VALUE
      setDelimeters(": ", "\r");
      setWhiteSpace("\n");        //a_Ignore \n, only accept \r
      m_valueWrap = '\x0';
    break;

    case ANameValuePair::COOKIE :
    case ANameValuePair::FORM_MULTIPART :
      //a_NAME=VALUE; NAME;
      setDelimeters("=", ";\r\n");
      setWhiteSpace(" \t");
      m_valueWrap = '\x0';
    break;

    case ANameValuePair::DATA :
      //a_NAME=VALUE;NAME;
      setDelimeters("=", ";\r\n");
      setWhiteSpace(" \t");
      m_valueWrap = '\x0';
    break;

    case ANameValuePair::CUSTOM : 
      //a_User will set it
      m_valueWrap = '\x0';
    break;

    default:
      ATHROW(this, AException::UnknownType);
  }
}

void ANameValuePair::setDelimeters(const AString &strExactSeparator, const AString &strOneOfTerminators)
{
  if (strExactSeparator.isEmpty() || strOneOfTerminators.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  m_separator  = strExactSeparator;
  m_terminator = strOneOfTerminators;  
}

void ANameValuePair::setValueWrap(char valueWrap)
{
  m_valueWrap = valueWrap;
}

void ANameValuePair::setName(const AString& name)
{
  if (name.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  m_name = name;
	
	switch (m_nvtype)
  {
    case ANameValuePair::HTML:
    case ANameValuePair::COOKIE:
    case ANameValuePair::FORM_MULTIPART:
    case ANameValuePair::DATA:
    case ANameValuePair::CGI:
  	  m_name.makeLower();
    break;
  }
}

void ANameValuePair::setNameValue(const AString& name, const AString& strValue)
{
  if (name.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  m_name  = name;
  m_value = strValue;

	switch (m_nvtype)
  {
    case ANameValuePair::HTML:
    case ANameValuePair::COOKIE:
    case ANameValuePair::DATA:
    case ANameValuePair::FORM_MULTIPART:
    case ANameValuePair::CGI:
  	  m_name.makeLower();
    break;
  }
}

void ANameValuePair::parse(const AString& strInput, size_t& pos)
{
  //a_Reset values
  clear();

  if (pos >= strInput.getSize())
    ATHROW_EX(this, AException::IndexOutOfBounds, AString("Parsing name '")+strInput+"' at pos="+AString::fromSize_t(pos));

  //a_First skip over whitespace
  pos = strInput.findNotOneOf(m_whitespace, pos);
  if (AConstant::npos == pos)
    return;

  //a_Parse the NAME parameter depending on type
  switch(m_nvtype)
  {
    case ANameValuePair::REGINI:
      _parseREGININame(strInput, pos, m_name);
    break;

    case ANameValuePair::XML:
    case ANameValuePair::HTML:
    case ANameValuePair::JSON:
    case ANameValuePair::CGI: 
    case ANameValuePair::CGI_CASE: 
    case ANameValuePair::HTTP:
    case ANameValuePair::COOKIE:
    case ANameValuePair::FORM_MULTIPART:
    case ANameValuePair::DATA:
    case ANameValuePair::CUSTOM:
      _parseName(strInput, pos, m_name);
    break;

    default:
      ATHROW(this, AException::UnknownType);
  
  }

  //a_Already ended
  if (pos >= strInput.getSize())
    return;

  //a_Parse the VALUE parameter depending on type
  switch(m_nvtype)
  {
    case ANameValuePair::XML:
    case ANameValuePair::HTML:
      _parseHtmlValue(strInput, pos, m_value);
    break;

    case ANameValuePair::REGINI:
      _parseREGINIValue(strInput, pos, m_value);
    break;

    case ANameValuePair::JSON:
      _parseJSONValue(strInput, pos, m_value);
    break;

    case ANameValuePair::CGI: 
    case ANameValuePair::CGI_CASE: 
    case ANameValuePair::HTTP:
    case ANameValuePair::COOKIE:
    case ANameValuePair::DATA:
    case ANameValuePair::CUSTOM:
    case ANameValuePair::FORM_MULTIPART:
      _parseValue(strInput, pos, m_value);
    break;

    default:
      ATHROW(this, AException::UnknownType);
  
  }

  //a_Perform conversions as needed by type
  switch(m_nvtype)
  {
    case ANameValuePair::XML:
    case ANameValuePair::HTTP: 
    case ANameValuePair::HTML:
    case ANameValuePair::JSON:
    case ANameValuePair::CUSTOM:
    case ANameValuePair::REGINI:
      //a_None or not applicable
    break;

    case ANameValuePair::CGI: 
    {
      //a_Decode URL after parsing (as special characters could throw off parsing)
      AString name(m_name), value(m_value);
      m_name.clear();
      m_value.clear();
      name.makeLower();
      ATextConverter::decodeURL(name, m_name);
      ATextConverter::decodeURL(value, m_value);
    }
    break;

    case ANameValuePair::FORM_MULTIPART:
    {
      //a_Decode URL after parsing (as special characters could throw off parsing)
      AString name(m_name), value(m_value);
      value.stripLeading(AConstant::ASTRING_DOUBLEQUOTE);
      value.stripTrailing(AConstant::ASTRING_DOUBLEQUOTE);
      m_name.clear();
      m_value.clear();
      ATextConverter::decodeURL(name, m_name);
      ATextConverter::decodeURL(value, m_value);
    }
    break;  

    case ANameValuePair::CGI_CASE: 
    case ANameValuePair::COOKIE:
    case ANameValuePair::DATA:
    {
      //a_Decode URL after parsing (as special characters could throw off parsing)
      AString name(m_name), value(m_value);
      m_name.clear();
      m_value.clear();
      ATextConverter::decodeURL(name, m_name);
      ATextConverter::decodeURL(value, m_value);
    }
    break;  

    default:
      ATHROW(this, AException::UnknownType);
  }

  return;
}

void ANameValuePair::_parseName(const AString &strInput, size_t& pos, AString &strReturn)
{
  strReturn.clear();

  size_t endSeparator = strInput.findOneOf(m_separator, pos);
  size_t endTerminator = strInput.findOneOf(m_terminator, pos);  
  if (AConstant::npos == endSeparator)
  {
    if (AConstant::npos == endTerminator)
    {
      // All of the input is name, no separator or terminator found
      strInput.peek(strReturn, pos);
      strReturn.stripTrailing(m_whitespace);
      pos = strInput.getSize() + 1;  
    }
    else
    {
      //a_Terminator found without separator
      if (endTerminator > pos)
      {
        strInput.peek(strReturn, pos, endTerminator - pos);
        strReturn.stripTrailing(m_whitespace);
      }
      pos = endTerminator;  
    }
  }
  else
  {
    if (endSeparator < endTerminator)
    {
      //a_Separator found before terminator
      strInput.peek(strReturn, pos, endSeparator - pos);
      strReturn.stripTrailing(m_whitespace);
      pos = endSeparator + m_separator.getSize();
    }
    else
    {
      //a_Terminator found before separator
      strInput.peek(strReturn, pos, endTerminator - pos);
      strReturn.stripTrailing(m_whitespace);
      pos = endTerminator;  
    }
  }
}

void ANameValuePair::_parseREGININame(const AString &strInput, size_t& pos, AString &strReturn)
{
  strReturn.clear();

  //a_Find terminator (which is the delimeter of this type)
  size_t start = strInput.findNotOneOf(m_terminator, pos++);
  if (AConstant::npos == start)
    ATHROW_EX(this, AException::InvalidData, ASWNL("Did not find start of name"));

  size_t end = strInput.find(m_separator, start+1);
  if (AConstant::npos == end)
  {
    strInput.peek(strReturn, start);
    end = strReturn.rfind(m_terminator);

    //a_Remove to terminator or strip trailing whitespace if none
    if (AConstant::npos != end)
      strReturn.setSize(end);
    else
      strReturn.stripTrailing(m_whitespace);

    pos = strInput.getSize() + 1;  // All of the input is name
  }
  else
  {
    strInput.peek(strReturn, start, end - start);
    size_t endName = strReturn.rfind(m_terminator);

    //a_Remove to terminator or strip trailing whitespace if none
    if (AConstant::npos != endName)
      strReturn.setSize(endName);
    else
      strReturn.stripTrailing(m_whitespace);

    pos = end + m_separator.getSize();
  }
}

void ANameValuePair::_parseValue(const AString &strInput, size_t& pos, AString &strReturn)
{
  //a_Reset value
  strReturn.clear();

  //a_Find the terminator
  size_t end = strInput.findOneOf(m_terminator, pos);
  if (end == AConstant::npos)
  {
    //a_No terminator (must be the last one)
    strInput.peek(strReturn, pos);
    pos = strInput.getSize() + 1;
  }
  else if (end == pos)
  {
    //a_Terminator at this position, there is no value
    ++pos;
  }
  else
  {
    strInput.peek(strReturn, pos, end - pos);
    pos = end + 1;
  }
}

void ANameValuePair::_parseREGINIValue(const AString &strInput, size_t& pos, AString &strReturn)
{
  //a_Reset value
  strReturn.clear();

  //a_Find the first "
  size_t start = strInput.findNotOneOf(m_whitespace, pos++);
  if (AConstant::npos != start)
  {
    //a_Ending char must be " or ' or a whitespace if neither
    char startChar = strInput.at(start);
    size_t end = AConstant::npos;
    if ('\"' == startChar || '\'' == startChar)
    {
      //a_Quoted with ' or "
      end = strInput.find(startChar, start + 1);
      ++start;
    }
    else
    {
      //a_Non-quoted value
      end = strInput.findOneOf(m_whitespace, start);
    }
    if (AConstant::npos == end)
    {
      //a_No terminator (must be the last one)
      strInput.peek(strReturn, start);
      pos = strInput.getSize() + 1;
    }
    else
    {
      AASSERT(this, start <= end);
      strInput.peek(strReturn, start, end - start);
      pos = end + 1;
    }
  }
}

void ANameValuePair::_parseJSONValue(const AString &strInput, size_t& pos, AString &strReturn)
{
  //a_Reset value
  strReturn.clear();

  //a_Find the start
  size_t start = strInput.findOneOf(m_terminator, pos++);
  if (AConstant::npos != start)
  {
    //a_Ending char must be " ... " or ' .. " .. '
    char startChar = strInput.at(start);
    size_t end = strInput.find(startChar, start + 1);
    if (AConstant::npos == end)
    {
      //a_No terminator (must be the last one)
      strInput.peek(strReturn, start + 1);
      pos = strInput.getSize() + 1;
    }
    else
    {
      strInput.peek(strReturn, start + 1, end - start - 1);
      pos = end + 1;
    }
    m_valueWrap = startChar;
  }

  return;
}

void ANameValuePair::_parseHtmlValue(const AString &strInput, size_t& pos, AString &strReturn)
{
  //a_Reset value
  strReturn.clear();
  size_t length = strInput.getSize();

  //a_Remove initial white space
  pos = strInput.findNotOneOf(m_whitespace, pos);
  if (AConstant::npos == pos)
  {
    //a_All value is whitespace
    return;
  }
  if (pos >= length)
    ATHROW_EX(this, AException::InvalidData, ASWNL("Unable to find terminator scanning for value"));

  char cInsideQuotes = ' ';  //a_Will be either ' or " if inside a quotes, empty otherwise
  bool boolSlash = false;
  char cX;

  std::deque<char> stackD;                       //a_Delimeter stack used for parsing complex values

  m_valueWrap = '\x0';
  while (pos < length)
  {
    cX = strInput[pos++];

    if (
         (__isWhiteSpace(cX)) &&
         (cInsideQuotes == ' ')
       )
      break;     //a_We are done, next item to follow

    switch (cX)
    {
      case '<' : 
        boolSlash = false;
        stackD.push_front('>');     //a_Add to the stack the end tag for popping later

        strReturn += cX;
      break;

      //a_Toggle in/out of quoted area
      case '\"' :
        if (!m_valueWrap)
          m_valueWrap = '\"';

        if (cInsideQuotes != '\'')
        {
          boolSlash = false;
          if (stackD.empty() == false)
          {
            //a_If the quote last then remove it and then check if we are still quoted
            if (stackD[0x0] == '\"')
            {
              stackD.pop_front();
              if (stackD.empty())
                break;
              if (std::find(stackD.begin(), stackD.end(), '\"') == stackD.end())
                cInsideQuotes = ' ';
            }
            else
            {
              //a_Just add it
              stackD.push_front('\"');     //a_Add to the stack
              cInsideQuotes = '\"';
            }
          }
          else
          {
            stackD.push_front('\"');     //a_Add to the stack
            cInsideQuotes = '\"';
            break;                       //a_Do not add to value
          }
        }
        //a_Add quotes
        strReturn += cX;
      break;

      case '\'' :
        if (!m_valueWrap)
          m_valueWrap = '\'';

        if (cInsideQuotes != '\"')
        {
          boolSlash = false;
          if (stackD.empty() == false)
          {
            //a_If the quote last then remove it and then check if we are still quoted
            if (stackD[0x0] == '\'')
            {
              stackD.pop_front();
              if (stackD.empty())
                break;
              if (std::find(stackD.begin(), stackD.end(), '\'') == stackD.end())
                cInsideQuotes = ' ';
            }
            else
            {
              //a_Just add it
              stackD.push_front('\'');     //a_Add to the stack
              cInsideQuotes = '\'';
            }
          }
          else
          {
            stackD.push_front('\'');     //a_Add to the stack
            cInsideQuotes = '\'';
            break;                       //a_Do not add to value
          }
        }

        //a_Add quotes
        strReturn += cX;
      break;

      //a_End of HTML tag found eg. NAME=VALUE> or NAME=VALUE/>
      case '/' : 
        boolSlash = true;
        strReturn += cX;
      break;

      case '>' :
      {
        //a_Top value is checked and quotes are then inserted
        if (stackD.empty() == false)
        {
          if(stackD[0x0] == '\"')
          {
            ATHROW_EX(this, AException::InvalidData, AString("Mismatch of \" and > while parsing '")+strInput+"' near pos="+AString::fromSize_t(pos));
          }
          else
          {
            strReturn += stackD[0x0];
            stackD.pop_front();
            break;
          }
        }
        else
        {
          //a_Stack is empty, do not add the '>'
          //a_If slash was last remove it
          if (boolSlash)
          {
            strReturn.remove(1, strReturn.getSize()-1);
            break;
          }
        }
        
        //a_End tag
        strReturn += cX;
      }
      break;
      
      default   : 
        boolSlash = false;
        strReturn += cX;
    }
    if (!m_valueWrap)
      m_valueWrap = '\"';  //a_HTML/XML require " wrapped values (or ' if " is part of value)
  
    //a_If the stack is empty and we are at one of "'>, then we are done parsing
    if ((stackD.empty()) && (cX == '>' || cX == '\"' || cX == '\''))
    {
      if (cX == '>')
        --pos;      //a_Back up to '>' or '/' so that the parent may exit
      if (boolSlash)
        --pos;      //a_Back up over /
      break;
    }
  }

  if (pos >= length)
  {
    if (!stackD.empty())
    {
      //a_Unwind the local stack :)
      while (stackD.empty() == false)
      {
        strReturn += stackD[0x0];
        stackD.pop_front();
      }
      ATHROW_EX(this, AException::EndOfBuffer, AString("Not enough > while parsing '")+strInput+"' near pos="+AString::fromSize_t(pos));
    }
  }
  
  return;
}

void ANameValuePair::setValue(const AString &strValue)
{ 
  size_t pos = 0;

  //a_Parse the VALUE parameter depending on type
  switch(m_nvtype)
  {
    case ANameValuePair::XML:
    case ANameValuePair::HTML:
      _parseHtmlValue(strValue, pos, m_value);
    break;

    case ANameValuePair::REGINI:
    case ANameValuePair::CGI: 
    case ANameValuePair::CGI_CASE: 
    case ANameValuePair::HTTP:
    case ANameValuePair::COOKIE:
    case ANameValuePair::FORM_MULTIPART:
    case ANameValuePair::DATA:
    case ANameValuePair::JSON:
    case ANameValuePair::CUSTOM:
      _parseValue(strValue, pos, m_value);
    break;

    default:
      ATHROW(this, AException::UnknownType);
  
  }

  //a_Perform conversions as needed by type
  switch(m_nvtype)
  {
    case ANameValuePair::XML:
    case ANameValuePair::HTTP: 
    case ANameValuePair::HTML:
    case ANameValuePair::CUSTOM:
    case ANameValuePair::JSON:
    case ANameValuePair::REGINI:
      //a_None or not applicable
    break;

    case ANameValuePair::FORM_MULTIPART:
    {
      //a_Decode URL after parsing (as special characters could throw off parsing)
      AString name(m_name), value(m_value);
      value.stripLeading(AConstant::ASTRING_DOUBLEQUOTE);
      value.stripTrailing(AConstant::ASTRING_DOUBLEQUOTE);
      m_name.clear();
      m_value.clear();
      ATextConverter::decodeURL(name, m_name);
      ATextConverter::decodeURL(value, m_value);
    }
    break;

    case ANameValuePair::CGI: 
    case ANameValuePair::CGI_CASE: 
    case ANameValuePair::COOKIE:
    case ANameValuePair::DATA:
    {
      //a_Decode URL after parsing (as special characters could throw off parsing)
      AString name(m_name), value(m_value);
      m_name.clear();
      m_value.clear();
      ATextConverter::decodeURL(name, m_name);
      ATextConverter::decodeURL(value, m_value);
    }
    break;  

    default:
      ATHROW(this, AException::UnknownType);
  }
}

void ANameValuePair::setWhiteSpace(const AString &str)
{ 
  m_whitespace = str;
}

bool ANameValuePair::__isWhiteSpace(char cX)
{ 
  return ((m_whitespace.find(cX) == AConstant::npos) ? false : true);
}
