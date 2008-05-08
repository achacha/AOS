#include "pchABase.hpp"

#include "AXmlAttributes.hpp"
#include "ANameValuePair.hpp"
#include "AOutputBuffer.hpp"

void AXmlAttributes::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Pairs={" << std::endl;
  
  for (AXmlAttributes::CONTAINER::const_iterator cit = m_Pairs.begin(); cit !=  m_Pairs.end(); ++cit)
  {
    AString str;
    (*cit)->emit(str);
    ADebugDumpable::indent(os, indent+2) << str << std::endl;
  }

  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

//a_Ctors and dtor
AXmlAttributes::AXmlAttributes()
{
}

AXmlAttributes::AXmlAttributes(const AXmlAttributes& that)
{
  append(that);
}

AXmlAttributes::AXmlAttributes(const AString &strLine)
{
  parse(strLine);
}

AXmlAttributes::~AXmlAttributes()
{
  try
  {
    clear();
  }
  catch(...) {}
}

void AXmlAttributes::append(const AXmlAttributes &that)
{
  for (AXmlAttributes::CONTAINER::const_iterator cit = that.m_Pairs.begin(); cit !=  that.m_Pairs.end(); ++cit)
  {
    m_Pairs.push_back(new ANameValuePair(*(*cit)));
  }
}

void AXmlAttributes::toAFile(AFile &aFile) const
{
  AString str(1024, 1024);
  emit(str);
  str.toAFile(aFile);
}

void AXmlAttributes::fromAFile(AFile &aFile)
{
  AString str(1024, 1024);
  str.fromAFile(aFile);
  parse(str);
}

void AXmlAttributes::emit(AOutputBuffer& target) const
{
  AXmlAttributes::CONTAINER::const_iterator cit = m_Pairs.begin();
  while (cit != m_Pairs.end())
  {
    (*cit)->emit(target);
    ++cit;
    if (cit != m_Pairs.end())
      target.append(AConstant::ASTRING_SPACE);
  }
}

ANameValuePair *AXmlAttributes::_findPair(const AString& name)
{
  for (AXmlAttributes::CONTAINER::iterator it = m_Pairs.begin(); it != m_Pairs.end(); ++it)
  {
    if ((*it)->getName().equalsNoCase(name))
      return (*it);
  }
  return NULL;
}

void AXmlAttributes::remove(const AString& name)
{
  for (AXmlAttributes::CONTAINER::iterator it = m_Pairs.begin(); it != m_Pairs.end(); ++it)
  {
    if ((*it)->getName().equalsNoCase(name))
    {
      delete *it;
      m_Pairs.erase(it);
      return;
    }
  }
}

const ANameValuePair *AXmlAttributes::_findPair(const AString& name) const
{
  for (AXmlAttributes::CONTAINER::const_iterator cit = m_Pairs.begin(); cit != m_Pairs.end(); ++cit)
  {
    if ((*cit)->getName().equalsNoCase(name))
      return (*cit);
  }
  return NULL;
}

bool AXmlAttributes::get(const AString& name, AOutputBuffer& target) const
{
  const ANameValuePair *pPair = _findPair(name);
  if (pPair)
  {
    target.append(pPair->getValue());
    return true;
  }
  else
    return false;
}

void AXmlAttributes::insert(const AString& name, const AString& value /* = AConstant::ASTRING_EMPTY */)
{
  AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AXmlAttributes);  //Debug only limit

  ANameValuePair *pPair = _findPair(name);
  if (pPair)
    pPair->setNameValue(name, value);
  else
    m_Pairs.push_back(new ANameValuePair(name, value, ANameValuePair::XML));
}

void AXmlAttributes::clear()
{
  for (AXmlAttributes::CONTAINER::const_iterator cit = m_Pairs.begin(); cit !=  m_Pairs.end(); ++cit)
  {
    delete (*cit);
  }
  m_Pairs.clear();
}

void AXmlAttributes::parse(const AString &strLine) 
{
  size_t pos = 0;
  ANameValuePair pair(ANameValuePair::XML);
  
  if (strLine.isEmpty())
    return;

  size_t length = strLine.getSize();
  while (pos < length)
  {
    pair.parse(strLine, pos);
    if (!pair.getName().isEmpty())
    {
      AASSERT(this, m_Pairs.size() < DEBUG_MAXSIZE_AXmlAttributes);  //Debug only limit
      m_Pairs.push_back(new ANameValuePair(pair));
    }

    pair.clear();
  }
}

size_t AXmlAttributes::getNames(SET_AString& names) const
{
  size_t ret = 0;
  for (AXmlAttributes::CONTAINER::const_iterator cit = m_Pairs.begin(); cit != m_Pairs.end(); ++cit, ++ret)
  {
    names.insert((*cit)->getName());
  }
  return ret;
}

const AXmlAttributes::CONTAINER& AXmlAttributes::getAttributeContainer() const
{
  return m_Pairs;
}

size_t AXmlAttributes::size() const
{
  return m_Pairs.size();
}

bool AXmlAttributes::getBool(const AString& name, bool defaultValue) const
{
  const ANameValuePair *pPair = _findPair(name);
  return (pPair ? pPair->getValue().toBool() : defaultValue);
}
