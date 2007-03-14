#include "pchABase.hpp"

#include "AAttributes.hpp"
#include "ANameValuePair.hpp"
#include "AException.hpp"
#include "AOutputBuffer.hpp"

#ifdef __DEBUG_DUMP__
void AAttributes::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AAttributes @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Pairs={" << std::endl;
  
  LIST_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit !=  m_Pairs.end())
  {
    AString str;
    (*cit).emit(str);
    ADebugDumpable::indent(os, indent+2) << str << std::endl;
    ++cit;
  }

  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

//a_Ctors and dtor
AAttributes::AAttributes()
{
}

AAttributes::AAttributes(const AString &strLine)
{
  parse(strLine);
}

AAttributes::~AAttributes()
{
}

void AAttributes::append(const AAttributes &that)
{
  LIST_NVPair::const_iterator cit = that.m_Pairs.begin();
  while (cit !=  that.m_Pairs.end())
  {
    m_Pairs.push_back(*cit);
    ++cit;
  }
}

void AAttributes::toAFile(AFile &aFile) const
{
  AString str(1024, 1024);
  emit(str);
  str.toAFile(aFile);
}

void AAttributes::fromAFile(AFile &aFile)
{
  AString str(1024, 1024);
  str.fromAFile(aFile);
  parse(str);
}

void AAttributes::emit(AOutputBuffer& target) const
{
  LIST_NVPair::const_iterator cit = m_Pairs.begin();
  if (cit != m_Pairs.end())
  {
    (*cit).emit(target);
    ++cit;
    while (cit != m_Pairs.end())
    {
      target.append(AString::sstr_Space);
      (*cit).emit(target);
      ++cit;
    }
  }
}

bool AAttributes::get(const AString& strName, AString& target, bool boolNoCase /* = true */) const
{
  target.clear();

  LIST_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    if (boolNoCase)
    {
      if ((*cit).isNameNoCase(strName))
      {
        target.assign((*cit).getValue());
        return true;
      }
    }
    else
    {
      if ((*cit).isName(strName))
      {
        target.assign((*cit).getValue());
        return true;
      }
    }

    ++cit;
  }
  return false;
}

size_t AAttributes::get(const AString& strName, LIST_AString& result, bool boolNoCase /* = true */) const
{
  size_t ret = 0;

  LIST_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    if (boolNoCase)
    {
      if ((*cit).isNameNoCase(strName))
      {
        result.push_back((*cit).getValue());
        ++ret;
      }
    }
    else
    {
      if ((*cit).isName(strName))
      {
        result.push_back((*cit).getValue());
        ++ret;
      }
    }

    ++cit;
  }
  return ret;
}

size_t AAttributes::count(const AString& strName, bool boolNoCase /* = true */) const
{
  size_t ret = 0;

  LIST_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    if (boolNoCase)
    {
      if ((*cit).isNameNoCase(strName))
      {
        ++ret;
      }
    }
    else
    {
      if ((*cit).isName(strName))
      {
        ++ret;
      }
    }

    ++cit;
  }
  return ret;
}

size_t AAttributes::getDelimited(const AString& strName, AString& result, const AString& delimeter /* = AString::sstr_Comma */, bool boolNoCase /* = true */) const
{
  size_t ret = 0;

  LIST_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    if (boolNoCase)
    {
      if ((*cit).isNameNoCase(strName))
      {
        result.append((*cit).getValue());
        result.append(delimeter);
        ++ret;
      }
    }
    else
    {
      if ((*cit).isName(strName))
      {
        result.append((*cit).getValue());
        result.append(delimeter);
        ++ret;
      }
    }

    ++cit;
  }

  if (ret > 0)
  {
    //a_Remove trailing comma is we found anything
    result.rremove();
  }

  return ret;
}

void AAttributes::remove(const AString& strName, const AString &strValue /*= AString::sstr_Empty */, bool boolNoCase /* = true */)
{
  LIST_NVPair::iterator it = m_Pairs.begin();
  while (it != m_Pairs.end())
  {
    if (boolNoCase)
    {
      if ((*it).isNameNoCase(strName))
      {
        if (strValue.isEmpty() || (*it).getValue().equals(strValue))
        {
          LIST_NVPair::iterator itKill = it;
          ++it;
          m_Pairs.erase(itKill);
        }
        else
          ++it;
      }
      else
        ++it;
    }
    else
    {
      if ((*it).isName(strName))
      {
        if (strValue.isEmpty() || (*it).getValue().equals(strValue))
        {
          LIST_NVPair::iterator itKill = it;
          ++it;
          m_Pairs.erase(itKill);
        }
        else
          ++it;
      }
      else
        ++it;
    }
  }
}

void AAttributes::insert(const AString& strName, const AString& strValue /* = AString::sstr_Empty */, ANameValuePair::NameValueType eType /* = ANameValuePair::XML */)
{
  m_Pairs.push_back(ANameValuePair(strName, strValue, eType));
}

void AAttributes::clear()
{
  m_Pairs.clear();
}

void AAttributes::parse(const AString &strLine, ANameValuePair::NameValueType eType /* = ANameValuePair::XML */) 
{
  size_t pos = 0;
  ANameValuePair pair(eType);
  
  if (strLine.isEmpty())
    return;

  size_t length = strLine.getSize();
  while (pos < length)
  {
    pair.parse(strLine, pos);
    if (!pair.getName().isEmpty())
      m_Pairs.push_back(pair);

    pair.clear();
  }
}

size_t AAttributes::getNames(SET_AString& names, bool boolNoCase /* = true */) const
{
  size_t ret = 0;

  LIST_NVPair::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    if (boolNoCase)
    {
      AString str((*cit).getName());
      str.makeLower();
      names.insert(str);
      ++ret;
    }
    else
    {
      names.insert((*cit).getName());
      ++ret;
    }

    ++cit;
  }

  return ret;
}

const LIST_NVPair& AAttributes::getAttributeContainer() const
{
  return m_Pairs;
}

size_t AAttributes::size() const
{
  return m_Pairs.size();
}