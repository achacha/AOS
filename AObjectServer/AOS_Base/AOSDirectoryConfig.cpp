
#include "pchAOS_Base.hpp"
#include "AOSDirectoryConfig.hpp"
#include "AOSCommand.hpp"
#include "AOSAdminRegistry.hpp"
#include "AXmlElement.hpp"
#include "ALog.hpp"

const AString AOSDirectoryConfig::CLASS("AOSDirectoryConfig");

#ifdef __DEBUG_DUMP__
void AOSDirectoryConfig::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSDirectoryConfig @ " << std::hex << this << std::dec << ") {" << std::endl;
  AOSAdminInterface::debugDump(os, indent+1);

  m_Modules.debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSDirectoryConfig::AOSDirectoryConfig(const AString path, const AXmlElement& base, ALog& log) :
  m_Path(path),
  m_Log(log)
{
  fromXml(base);
}

AOSDirectoryConfig::~AOSDirectoryConfig()
{
}

const AString& AOSDirectoryConfig::getClass() const
{
  return CLASS;
}

void AOSDirectoryConfig::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  //a_Display the command name (which is the path)
  eBase.addAttribute(ASW("display",7), m_Path);

  ARope rope;
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.use().begin();
  int i=0;
  while (cit != m_Modules.use().end())
  {
    ARope ropeName("module.", 7);
    ropeName.append(AString::fromInt(i));
    addProperty(eBase, ropeName, (*cit)->getModuleClass());
    ropeName.append(".params",7);

    rope.clear();
    (*cit)->useParams().emit(rope, 0);
    
    addProperty(eBase, ropeName, rope, AXmlData::CDataSafe);
    ++cit;
    ++i;
  }
}

void AOSDirectoryConfig::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
}

void AOSDirectoryConfig::registerAdminObject(AOSAdminRegistry& registry)
{
  AString str(getClass());
  str.append(':');
  str.append(m_Path);
  registry.insert(str, *this);
}

void AOSDirectoryConfig::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("AOSDirectoryConfig",18));

  //a_Modules
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.get().begin();
  while (cit != m_Modules.get().end())
  {
    AXmlElement& eModule = target.addElement(ASW("module",6));
    eModule.addAttribute(AOSCommand::CLASS, (*cit)->getModuleClass());
    if ((*cit)->getParams().hasElements())
      eModule.addContent((*cit)->getParams().clone());
    ++cit;
  }
}

void AOSDirectoryConfig::fromXml(const AXmlElement& element)
{
  AString str(256,128);

  //a_Get module names
  m_Modules.use().clear();
  AXmlNode::ConstNodeContainer nodes;
  element.find(ASW("module",6), nodes);
  AXmlNode::ConstNodeContainer::const_iterator citModule = nodes.begin();
  AString strClass, strName;
  while(citModule != nodes.end())
  {
    //a_Get module 'class', this is the registered MODULE_CLASS and get module params
    if ((*citModule)->getAttributes().get(ASW("class",5), strClass) && !strClass.isEmpty())
    {
      //a_Add new module info object
      const AXmlElement *pBase = dynamic_cast<const AXmlElement *>(*citModule);
      AASSERT(*citModule, pBase);
      m_Modules.use().push_back(new AOSModuleInfo(strClass, *pBase));
    }
    ++citModule;
  }
}

const AOSModules& AOSDirectoryConfig::getModules() const
{
  return m_Modules;
}

const AString& AOSDirectoryConfig::getPath() const
{ 
  return m_Path;
}
