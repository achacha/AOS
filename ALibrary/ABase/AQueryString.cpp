/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "AQueryString.hpp"
#include "ANameValuePair.hpp"
#include "AFile.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
void AQueryString::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AQueryString @ " << std::hex << this << std::dec << ") {" << std::endl;

  MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    AString str;
    (*cit).second.emit(str);
    ADebugDumpable::indent(os, indent+1) << "first=" << (*cit).first << "  second='" << str << "'" << std::endl;
    ++cit;
  }

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AQueryString::AQueryString()
{
}

AQueryString::AQueryString(const AQueryString &that)
{
  if (this != &that)
    copy(that);
}

AQueryString::AQueryString(const AString &strLine)
{
  parse(strLine);
}

AQueryString::~AQueryString()
{
}

AQueryString &AQueryString::operator =(const AQueryString& that)
{ 
  if (this != &that)
    copy(that); 

  return *this;
}

void AQueryString::copy(const AQueryString &that)
{
  m_Pairs.clear();
  MMAP_AString_NVPair::const_iterator cit = that.m_Pairs.begin();
  while (cit !=  that.m_Pairs.end())
  {
    m_Pairs.insert(MMAP_AString_NVPair::value_type((*cit).first, (*cit).second));
    ++cit;
  }
}

void AQueryString::append(const AQueryString &that)
{
  MMAP_AString_NVPair::const_iterator cit = that.m_Pairs.begin();
  while (cit !=  that.m_Pairs.end())
  {
    m_Pairs.insert(MMAP_AString_NVPair::value_type((*cit).first, (*cit).second));
    ++cit;
  }
}

void AQueryString::toAFile(AFile &aFile) const
{
  AString str(2048, 1024);
  emit(str);
  aFile.write(str);
}

void AQueryString::fromAFile(AFile &aFile)
{
  AString str(2048, 1024);
  aFile.readLine(str);       //a_Just a guess that it is until end of line
  parse(str);
}

size_t AQueryString::size() const
{
  return m_Pairs.size();
}

void AQueryString::emit(AOutputBuffer& target) const
{
  MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  if (cit != m_Pairs.end())
  {
    (*cit).second.emit(target);
    ++cit;
    while (cit != m_Pairs.end())
    {
      target.append('&');
      (*cit).second.emit(target);
      ++cit;
    }
  }
}

AXmlElement& AQueryString::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  thisRoot.addAttribute(ASW("size",4), u8(m_Pairs.size()));
  for(MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin(); cit != m_Pairs.end(); ++cit)
    thisRoot.addElement((*cit).second.getName()).addData((*cit).second.getValue(), AXmlElement::ENC_CDATASAFE);

  return thisRoot;
}

size_t AQueryString::get(const AString& name, LIST_NVPair& result) const
{
  size_t ret = 0;
  result.clear();

  AString str(name);
  str.makeLower();

  std::pair<MMAP_AString_NVPair::const_iterator, MMAP_AString_NVPair::const_iterator> iters = m_Pairs.equal_range(str);
  while (iters.first != iters.second)
  {
    result.push_back((*(iters.first)).second);
    ++(iters.first);
    ++ret;
  }

  return ret;
}

bool AQueryString::get(const AString& name, AString& target) const
{
  target.clear();
  AString str(name);
  str.makeLower();

  MMAP_AString_NVPair::const_iterator cit = m_Pairs.find(str);
  if (cit != m_Pairs.end())
  {
    target.assign((*cit).second.getValue());
    return true;
  }
  else
    return false;
}

void AQueryString::remove(const AString& name, const AString &value /*= AConstant::ASTRING_EMPTY */)
{
  AString str(name);
  str.makeLower();

  MMAP_AString_NVPair::iterator it = m_Pairs.find(str);
  if (it != m_Pairs.end())
  {
    if (value.isEmpty())
    {
      m_Pairs.erase(it);
    }
    else
    {
      MMAP_AString_NVPair::iterator killit;
      while (it != m_Pairs.end())
      {
        if ((*it).second.getValue() == value)
        {
          killit = it;
          ++it;
          m_Pairs.erase(killit, killit);
        }
        else  
          ++it;
      }
    }
  }
}

size_t AQueryString::count(const AString& name) const
{
  AString str(name);
  str.makeLower();

  return m_Pairs.count(str);
}

bool AQueryString::exists(const AString& name) const
{
  AString str(name);
  str.makeLower();

  return m_Pairs.count(str) > 0;
}

void AQueryString::insert(const AString &name, const AString &value)
{
  AString str(name);
  str.makeLower();

  m_Pairs.insert(MMAP_AString_NVPair::value_type(str, ANameValuePair(str, value, ANameValuePair::CGI)));
}

void AQueryString::clear()
{
  m_Pairs.clear();
}

void AQueryString::parse(
  const AString &strLine, 
  ANameValuePair::NameValueType type // = ANameValuePair::CGI
) 
{
  size_t pos = 0;
  ANameValuePair pair(type);
  
  if (strLine.isEmpty())
    return;

  if (strLine[0] == '?') 
    pos++;    //a_Still has the URL separator

  size_t length = strLine.getSize();
  while (pos < length)
  {
    pair.parse(strLine, pos);
    if (!pair.getName().isEmpty())
      m_Pairs.insert(MMAP_AString_NVPair::value_type(pair.getName(), pair));

    pair.clear();
  }
}

size_t AQueryString::getNames(SET_AString& names) const
{
  size_t ret = 0;
  MMAP_AString_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    names.insert((*cit).first);
    ++(cit);
    ++ret;
  }

  return ret;
}

size_t AQueryString::getDelimited(
  const AString& name, 
  AString& result, 
  const AString& delimeter // = AConstant::ASTRING_COMMA
) const
{
  size_t ret = 0;

  AString str(name);
  str.makeLower();

  std::pair<MMAP_AString_NVPair::const_iterator, MMAP_AString_NVPair::const_iterator> iters = m_Pairs.equal_range(str);
  while (iters.first != iters.second)
  {
    result.append((*(iters.first)).second.getValue());
    ++iters.first;
    if (iters.first != iters.second)
      result.append(delimeter);
    
    ++ret;
  }

  return ret;
}

void AQueryString::emitDelimeted(
  AOutputBuffer& target, 
  const AString& valueSeparator, 
  const AString& pairSeparator, 
  bool appendSeparatorAtEnd          // = true
) const
{
  MMAP_AString_NVPair::const_iterator it = m_Pairs.begin();
  while(it != m_Pairs.end())
  {
    target.append((*it).second.getName());
    target.append(valueSeparator);
    target.append((*it).second.getValue());
    ++it;
    
    if (appendSeparatorAtEnd || it != m_Pairs.end())
      target.append(pairSeparator);
  }
}
