
#include "pchABase.hpp"
#include "ACommandLineParser.hpp"

void ACommandLineParser::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "ExecutableName=" << m_ExecutableName << std::endl;
  ADebugDumpable::indent(os, indent+1) << "NamedPairs={" << std::endl;
  for(MAP_AString_AString::const_iterator cit = m_NamedPairs.begin(); cit != m_NamedPairs.end(); ++cit)
  {
    ADebugDumpable::indent(os, indent+2) << "[" << cit->first << "]=[" << cit->second << "]" << std::endl;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "AnonValues={" << std::endl;
  for(LIST_AString::const_iterator cit2 = m_AnonValues.begin(); cit2 != m_AnonValues.end(); ++cit2)
  {
    ADebugDumpable::indent(os, indent+2) << *cit2 << std::endl;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ACommandLineParser::ACommandLineParser(int argc, char **argv, AString delimiter) :
  m_Delimiter(delimiter)
{
  AASSERT(this, argc > 0);

  m_ExecutableName = argv[0];
  int i = 1;
  while (i < argc)
  {
    AString name(argv[i]);
    if (name.startsWith(m_Delimiter))
    {
      // Name starts with delimiter
      name.remove(m_Delimiter.getSize());
      ++i;

      AString value;
      if (i < argc)
      {
        value.assign(argv[i]);
        if (value.startsWith(m_Delimiter))
          value.clear();
        else
          ++i;
      }

      m_NamedPairs[name] = value;
    }
    else
    {
      // Anon value
      m_AnonValues.push_back(name);
      ++i;
    }
  }
}

void ACommandLineParser::emit(AOutputBuffer& target) const
{
  target.append(m_ExecutableName);
  for(MAP_AString_AString::const_iterator cit = m_NamedPairs.begin(); cit != m_NamedPairs.end(); ++cit)
  {
    target.append(' ');
    target.append(m_Delimiter);
    target.append(cit->first);
    if (cit->second.isNotEmpty())
    {
      target.append(' ');
      target.append(cit->second);
    }
  }
  for(LIST_AString::const_iterator cit2 = m_AnonValues.begin(); cit2 != m_AnonValues.end(); ++cit2)
  {
    target.append(' ');
    target.append(*cit2);
  }
}

void ACommandLineParser::emitFilename(AOutputBuffer& target) const
{
  target.append(m_ExecutableName);
}

void ACommandLineParser::emitNamedParamaters(AOutputBuffer& target) const
{
  MAP_AString_AString::const_iterator cit = m_NamedPairs.begin();
  while(cit != m_NamedPairs.end())
  {
    target.append(m_Delimiter);
    target.append(cit->first);
    if (cit->second.isNotEmpty())
    {
      target.append(' ');
      target.append(cit->second);
    }
    ++cit;
    if (cit != m_NamedPairs.end())
      target.append(' ');
  }
}

void ACommandLineParser::emitAnonValues(AOutputBuffer& target) const
{
  LIST_AString::const_iterator cit = m_AnonValues.begin();
  while(cit != m_AnonValues.end())
  {
    target.append(*cit);
    ++cit;
    if (cit != m_AnonValues.end())
      target.append(' ');
  }
}

bool ACommandLineParser::existsNamedValue(const AString& name) const
{
  MAP_AString_AString::const_iterator cit = m_NamedPairs.find(name);
  return (cit != m_NamedPairs.end());
}

bool ACommandLineParser::getNamedValue(const AString& name, AOutputBuffer& value) const
{
  MAP_AString_AString::const_iterator cit = m_NamedPairs.find(name);
  if (cit != m_NamedPairs.end())
  {
    value.append(cit->second);
    return true;
  }
  return false;
}

bool ACommandLineParser::getNamedValue(const AString& name, AOutputBuffer& value, const AString& defaultValue) const
{
  bool ret = getNamedValue(name, value);
  if (!ret)
  {
    value.append(defaultValue);
  }
  return ret;
}

bool ACommandLineParser::existsAnonValue(const AString& name) const
{
  for(LIST_AString::const_iterator cit = m_AnonValues.begin(); cit != m_AnonValues.end(); ++cit)
  {
    if ((*cit).equals(name))
      return true;
  }
  return false;
}

size_t ACommandLineParser::size() const
{
  return m_AnonValues.size() + m_NamedPairs.size();
}

AString& ACommandLineParser::useExecutableName()
{
  return m_ExecutableName;
}

MAP_AString_AString& ACommandLineParser::useNamedPairs()
{
  return m_NamedPairs;
}

LIST_AString& ACommandLineParser::useAnonValues()
{
  return m_AnonValues;
}

const AString& ACommandLineParser::getExecutableName() const
{
  return m_ExecutableName;
}

const MAP_AString_AString& ACommandLineParser::getNamedPairs() const
{
  return m_NamedPairs;
}

const LIST_AString& ACommandLineParser::getAnonValues() const
{
  return m_AnonValues;
}
