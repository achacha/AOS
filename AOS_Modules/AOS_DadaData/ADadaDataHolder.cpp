/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_DadaData.hpp"
#include "ADadaDataHolder.hpp"
#include "AFile_Physical.hpp"
#include "AFilename.hpp"

void ADadaDataHolder::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << ") @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_wordmap={" << std::endl;
  WORDMAP::const_iterator citW = m_wordmap.begin();
  while (citW != m_wordmap.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*citW).first << " size=" << u4((*citW).second.size()) << std::endl;
    ++citW;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
    
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ADadaDataHolder::ADadaDataHolder()
{
}

ADadaDataHolder::ADadaDataHolder(const ADadaDataHolder&)
{
}

ADadaDataHolder::~ADadaDataHolder()
{
}

void ADadaDataHolder::emit(AOutputBuffer& target) const
{
  WORDMAP::const_iterator cit = m_wordmap.begin();
  size_t i;
  while (m_wordmap.end() != cit)
  {
    const VECTOR_AString& vec = (*cit).second;
    for (i=0; i<vec.size()-1; ++i)
    {
      target.append(vec.at(i));
      target.append(',');
    }
    target.append(vec.at(i));
    ++cit;
  }
}

AXmlElement& ADadaDataHolder::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());
  
  WORDMAP::const_iterator cit = m_wordmap.begin();
  while (m_wordmap.end() != cit)
  {
    AXmlElement& elemType = thisRoot.addElement(ASW("type",4));
    AXmlElement& elem = elemType.addElement((*cit).first);
    const VECTOR_AString& vec = (*cit).second;
    for (size_t i=0; i<vec.size(); ++i)
    {
      elem.addElement(ASW("word",4), vec.at(i));
    }
    ++cit;
  }

  return thisRoot;
}

void ADadaDataHolder::resetData()
{
  m_wordmap.clear();
}

void ADadaDataHolder::readData(AOSServices& services, const AXmlElement *pSet)
{
  AXmlElement::CONST_CONTAINER nodes;
  AString str;
  pSet->find(ASW("data",4), nodes);
  AXmlElement::CONST_CONTAINER::iterator it = nodes.begin();
  while (it != nodes.end())
  {
    str.clear();
    (*it)->emitContent(str);
    
    AFilename filename(services.useConfiguration().getAosBaseDataDirectory(), str, false);
    if (AFileSystem::exists(filename))
    {
      str.clear();
      if ((*it)->getAttributes().get(ASW("type",4), str))
      {
        _loadWords(str, filename, services.useLog());
      }
      else
        services.useLog().add(ASWNL("AOS_DadaData: <data> missing 'type' attribute"), ALog::FAILURE);
    }
    else
      services.useLog().add(ARope("Missing word file: ")+filename, ALog::WARNING);
    
    ++it;
  }
}

void ADadaDataHolder::_loadWords(const AString& type, const AFilename& filename, ALog& alog)
{
  WORDMAP::iterator it = m_wordmap.find(type);
  if (m_wordmap.end() != it)
  {
    alog.append(AString("Duplicate type for '")+type+"' found in INI file, ignoring.\n");
    return;
  }
  
  AFile_Physical file(filename, ASW("r", 1));
  file.open();

  AString str;
  while (AConstant::npos != file.readLine(str))
  {
    if (!str.isEmpty() && '#' != str.at(0))
    {
      m_wordmap[type].push_back(str);
      str.clear();
    }
  }
}
