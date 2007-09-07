#include "pchAOS_DadaData.hpp"
#include "ADadaDataHolder.hpp"
#include "AINIProfile.hpp"
#include "AFile_Physical.hpp"
#include "AFilename.hpp"

#ifdef __DEBUG_DUMP__
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
  
  ADebugDumpable::indent(os, indent+1) << "m_templates={" << std::endl;
  TEMPLATES::const_iterator citT = m_templates.begin();
  while (citT != m_templates.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*citT).first << " size=" << u4((*citT).second.size()) << std::endl;
    ++citT;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif


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

void ADadaDataHolder::emit(AXmlElement& element) const
{
  WORDMAP::const_iterator cit = m_wordmap.begin();
  while (m_wordmap.end() != cit)
  {
    AXmlElement& elemType = element.addElement(ASW("type",4));
    AXmlElement& elem = elemType.addElement((*cit).first);
    const VECTOR_AString& vec = (*cit).second;
    for (size_t i=0; i<vec.size(); ++i)
    {
      elem.addElement(ASW("word",4), vec.at(i));
    }
    ++cit;
  }
}

void ADadaDataHolder::resetData()
{
  m_templates.clear();
  m_wordmap.clear();
}

void ADadaDataHolder::readData(
  const AFilename& iniFilename,
  const AFilename& baseDataDir,
  ALog& alog
)
{
  if (!AFileSystem::exists(iniFilename))
  {
    alog.add(AString("ERROR: ADadaDataHolder: unable to open INI profile"), iniFilename.toAString(), ALog::FAILURE);
    return;
  }

  AINIProfile iniProfile(iniFilename);
  iniProfile.parse();

  AINIProfile::WindingIterator it = iniProfile.getWindingIterator(ASW("dadadata",8));
  AString strName, strValue;
  while (it.next())
  {
    it.getName(strName);
    it.getValue(strValue);
    AFilename filename(baseDataDir);
    filename.join(strValue, false);
    _loadWords(strName, filename, alog);
  }

  it = iniProfile.getWindingIterator(ASW("templates", 9));
  while (it.next())
  {
    it.getName(strName);
    if ('#' != strName.at(0, '\x0'))
    {
      it.getValue(strValue);
      AFilename filename(baseDataDir);
      filename.join(strValue, false);
      _loadTemplate(strName, filename, alog);
    }
    else
      alog.add(ASWNL("ADadaDataHolder: skipping template"), strName, ALog::WARNING);
  }
}

void ADadaDataHolder::readData(AOSServices& services)
{
  AXmlNode::ConstNodeContainer nodes;

  //a_Words
  AString str;
  services.useConfiguration().getConfigRoot().find(ASWNL("/config/dada/data"), nodes);
  AXmlNode::ConstNodeContainer::iterator it = nodes.begin();
  while (it != nodes.end())
  {
    str.clear();
    (*it)->emitContent(str);
    AFilename filename(services.useConfiguration().getAosBaseDataDirectory(), str, false);
    
    str.clear();
    if ((*it)->getAttributes().get(ASW("type",4), str))
    {
      _loadWords(str, filename, services.useLog());
    }
    else
      services.useLog().add(ASWNL("AOS_DadaData: <data> missing 'type' attribute"), ALog::FAILURE);

    ++it;
  }

  //a_Templates
  nodes.clear();
  services.useConfiguration().getConfigRoot().find(ASWNL("/config/dada/template"), nodes);
  it = nodes.begin();
  while (it != nodes.end())
  {
    str.clear();
    (*it)->emitContent(str);
    AFilename filename(services.useConfiguration().getAosBaseDataDirectory(), str, false);
    
    str.clear();
    if ((*it)->getAttributes().get(ASW("name",4), str))
    {
      _loadTemplate(str, filename, services.useLog());
    }
    else
      services.useLog().add(ASWNL("AOS_DadaData: <template> missing 'name' attribute"), ALog::FAILURE);

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
  
  AFile_Physical file(filename.toAString(), ASW("r", 1));
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

void ADadaDataHolder::_loadTemplate(const AString& name, const AFilename& filename, ALog& alog)
{
  AFile_Physical file(filename.toAString(), ASW("r", 1));
  file.open();

  AString str;
  while (AConstant::npos != file.readLine(str))
  {
    if ('#' != str.at(0, '\x0'))
    {
      m_templates[name].push_back(str);
    }
    str.clear();
  }
}
