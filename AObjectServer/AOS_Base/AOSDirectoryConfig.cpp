/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSDirectoryConfig.hpp"
#include "AOSController.hpp"
#include "AOSAdminRegistry.hpp"
#include "AXmlElement.hpp"
#include "ALog.hpp"

const AString AOSDirectoryConfig::CLASS("AOSDirectoryConfig");

void AOSDirectoryConfig::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  AOSAdminInterface::debugDump(os, indent+1);

  m_Modules.debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

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

void AOSDirectoryConfig::adminEmitXml(AXmlElement& thisRoot, const AHTTPRequestHeader& request)
{
  //a_Display the command name (which is the path)
  thisRoot.addAttribute(ASW("display",7), m_Path);

  ARope rope;
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.use().begin();
  int i=0;
  while (cit != m_Modules.use().end())
  {
    ARope ropeName(AOSController::S_MODULE);
    ropeName.append('.');
    ropeName.append(AString::fromInt(i));
    adminAddProperty(thisRoot, ropeName, (*cit)->getModuleClass());
    ropeName.append(".params",7);

    rope.clear();
    (*cit)->useParams().emit(rope, 0);
    
    adminAddProperty(thisRoot, ropeName, rope, AXmlElement::ENC_CDATASAFE);
    ++cit;
    ++i;
  }
}

void AOSDirectoryConfig::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
}

void AOSDirectoryConfig::adminRegisterObject(AOSAdminRegistry& adminRegistry)
{
  AString str(getClass());
  str.append(':');
  str.append(m_Path);
  adminRegistry.insert(str, *this);
}

AXmlElement& AOSDirectoryConfig::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  //a_Modules
  for (AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.get().begin(); cit != m_Modules.get().end(); ++cit)
  {
    AXmlElement& eModule = thisRoot.addElement(AOSController::S_MODULE);
    eModule.addAttribute(AOSController::S_CLASS, (*cit)->getModuleClass());
    if ((*cit)->getParams().hasElements())
      eModule.addContent((*cit)->getParams().clone());
  }

  return thisRoot;
}

void AOSDirectoryConfig::fromXml(const AXmlElement& element)
{
  AString str(256,128);

  //a_Get module names
  m_Modules.use().clear();
  AXmlElement::CONST_CONTAINER nodes;
  element.find(AOSController::S_MODULE, nodes);
  
  AString strClass, strName;
  for(AXmlElement::CONST_CONTAINER::const_iterator citModule = nodes.begin(); citModule != nodes.end(); ++citModule)
  {
    //a_Get module 'class', this is the registered MODULE_CLASS and get module params
    if ((*citModule)->getAttributes().get(AOSController::S_CLASS, strClass) && !strClass.isEmpty())
    {
      //a_Add new module info object
      AASSERT(*citModule, *citModule);
      m_Modules.use().push_back(new AOSModuleInfo(strClass, *(*citModule)));
    }
    strClass.clear();
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
