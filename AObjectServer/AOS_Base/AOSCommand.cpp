
#include "pchAOS_Base.hpp"
#include "AOSCommand.hpp"
#include "AXmlDocument.hpp"
#include "ALog.hpp"
#include "AOSAdminRegistry.hpp"
#include "AOSModules.hpp"

const AString AOSCommand::CLASS("AOSCommand");

#ifdef __DEBUG_DUMP__
void AOSCommand::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSCommand @ " << std::hex << this << std::dec << ") {" << std::endl;
  AOSAdminInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent+1) << "m_Enabled=" << (m_Enabled ? "true" : "false") << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Session=" << (m_Session ? "true" : "false") << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ForceAjax=" << (m_ForceAjax ? "true" : "false") << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Command=" << m_Command << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_InputProcessor=" << m_InputProcessor << std::endl;
  if (m_InputParams.hasElements())
  {
    ADebugDumpable::indent(os, indent+1) << "m_InputParams=" << std::endl;
    m_InputParams.debugDump(os, indent+2);
  }
  else
  {
    ADebugDumpable::indent(os, indent+1) << "m_InputParams={}" << std::endl;
  }

  ADebugDumpable::indent(os, indent+1) << "m_OutputGenerator=" << m_OutputGenerator << std::endl;
  if (m_OutputParams.hasElements())
  {
    ADebugDumpable::indent(os, indent+1) << "m_OutputParams=" << std::endl;
    m_OutputParams.debugDump(os, indent+2);
  }
  else
    ADebugDumpable::indent(os, indent+1) << "m_OutputParams={}" << std::endl;

  m_Modules.debugDump(os, indent+1);
 
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSCommand::AOSCommand(const AString& name, ALog& log) :
  m_Command(name),
  m_InputParams(ASW("params",6)),
  m_OutputParams(ASW("params",6)),
  m_Enabled(true),
  m_Session(false),
  m_ForceAjax(false),
  m_Log(log)
{
}

AOSCommand::~AOSCommand()
{
}

const AString& AOSCommand::getClass() const
{
  return CLASS;
}

void AOSCommand::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  //a_Display the commadn name (which is the path)
  eBase.addAttribute(ASW("display",7), m_Command);

  ARope rope;
  if (!m_InputProcessor.isEmpty())
  {
    addProperty(eBase, ASW("inputProcessor",14), m_InputProcessor);
    m_InputParams.emit(rope, 0);
    addProperty(eBase, ASW("inputProcessor.params",21), rope, AXmlData::CDataSafe);
  }

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

  if (!m_OutputGenerator.isEmpty())
  {
    addProperty(eBase, ASW("outputGenerator",15), m_OutputGenerator);
    rope.clear();
    m_OutputParams.emit(rope, 0);
    addProperty(eBase, ASW("outputGenerator.params",22), rope, AXmlData::CDataSafe);
  }

  addPropertyWithAction(
    eBase, 
    ASW("enabled",7), 
    AString::fromBool(m_Enabled),
    ASW("Update",6), 
    ASWNL("Enable(1) or Disable(0) the command"),
    ASW("Set",3));
  
  addPropertyWithAction(
    eBase, 
    ASW("session",7), 
    AString::fromBool(m_Session),
    ASW("Update",6), 
    ASWNL("Use session(1) or No session(0)"),
    ASW("Set",3)
  );
  
  addPropertyWithAction(
    eBase, 
    ASW("forceAJAX",9), 
    AString::fromBool(m_ForceAjax),
    ASW("Update",6), 
    ASWNL("Force AJAX(1) or Default(0)"),
    ASW("Set",3)
  );
}

void AOSCommand::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("enabled",7), str))
  {
    m_Enabled = (str.at(0) == '0' ? false : true);
  }
  else if (request.getUrl().getParameterPairs().get(ASW("session",7), str))
  {
    m_Session = (str.at(0) == '0' ? false : true);
  }
  else if (request.getUrl().getParameterPairs().get(ASW("ajax",4), str))
  {
    m_ForceAjax = (str.at(0) == '0' ? false : true);
  }
}

void AOSCommand::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("AOSCommand",10));

  target.addAttribute(ASW("enabled", 7), m_Enabled ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
  target.addAttribute(ASW("session",7), m_Session ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
  target.addAttribute(ASW("forceAJAX",9), m_ForceAjax ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
  
  //a_Input
  AXmlElement& eInput = target.addElement(ASW("input_processor",15));
  eInput.addAttribute(ASW("class",5), m_InputProcessor);
  if (m_InputParams.hasElements()) 
    eInput.addContent(m_InputParams.clone());
  
  //a_Modules
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.get().begin();
  while (cit != m_Modules.get().end())
  {
    AXmlElement& eModule = target.addElement(ASW("module",6));
    eModule.addAttribute(ASW("class",5), (*cit)->getModuleClass());
    if ((*cit)->getParams().hasElements())
      eModule.addContent((*cit)->getParams().clone());
    ++cit;
  }

  //a_Output
  AXmlElement& eOutput = target.addElement(ASW("output_generator",16));
  eOutput.addAttribute(ASW("class",5), m_OutputGenerator);
  if (m_OutputParams.hasElements())
    eOutput.addContent(m_OutputParams.clone());
}

void AOSCommand::fromAXmlElement(const AXmlElement& element)
{
  AString str(256,128);

  //a_Set command name
  if (m_Command.isEmpty())
    element.getAttributes().get(ASW("name",4), m_Command);

  //a_Enabled?
  element.getAttributes().get(ASW("enabled",7), str);
  if (str.equals(AConstant::ASTRING_ZERO) || str.equals(AConstant::ASTRING_FALSE))
    m_Enabled = false;
  else
    m_Enabled = true;

  //a_Session based?
  element.getAttributes().get(ASW("session",7), str);
  if (str.equals(AConstant::ASTRING_ZERO) || str.equals(AConstant::ASTRING_FALSE))
    m_Session = false;
  else
    m_Session = true;

  //a_Is Ajax call?
  element.getAttributes().get(ASW("forceAJAX",9), str);
  if (str.equals(AConstant::ASTRING_ONE) || str.equals(AConstant::ASTRING_TRUE))
    m_ForceAjax = true;
  else
    m_ForceAjax = false;

  //a_Get input processor
  m_InputProcessor.clear();
  m_InputParams.clear();
  const AXmlNode *pNode = element.findNode(ASW("input",5));
  if (pNode)
  {
    pNode->getAttributes().get(ASW("class",5), m_InputProcessor);
    pNode->emitXml(m_InputParams);
  }

  //a_Get output generator
  m_OutputGenerator.clear();
  m_OutputParams.clear();
  pNode = element.findNode(ASW("output",6));
  if (pNode)
  {
    pNode->getAttributes().get(ASW("class",5), m_OutputGenerator);
    pNode->emitXml(m_OutputParams);
  }

  //a_Get module names
  m_Modules.use().clear();
  AXmlNode::ConstNodeContainer nodes;
  element.find(ASW("module",6), nodes);
  
  AString strClass;
  for (AXmlNode::ConstNodeContainer::const_iterator citModule = nodes.begin(); citModule != nodes.end(); ++citModule)
  {
    //a_Get module 'class', this is the registered MODULE_CLASS and get module params
    if ((*citModule)->getAttributes().get(ASW("class",5), strClass) && !strClass.isEmpty())
    {
      //a_Add new module info object
      const AXmlElement *pBase = dynamic_cast<const AXmlElement *>(*citModule);
      AASSERT(*citModule, pBase);
      m_Modules.use().push_back(new AOSModuleInfo(strClass, *pBase));
    }
    strClass.clear();
  }
}

void AOSCommand::registerAdminObject(AOSAdminRegistry& registry)
{
  AString str(getClass());
  str.append(':');
  str.append(m_Command);
  registry.insert(str, *this);
}

const AOSModules& AOSCommand::getModules() const
{
  return m_Modules;
}

const AXmlElement& AOSCommand::getInputParams() const
{
  return m_InputParams;
}

const AXmlElement& AOSCommand::getOutputParams() const
{
  return m_OutputParams;
}

const AString& AOSCommand::getCommandName() const
{ 
  return m_Command;
}

bool AOSCommand::isEnabled() const
{
  return m_Enabled;
}

bool AOSCommand::isSession() const
{
  return m_Session;
}

bool AOSCommand::isForceAjax() const
{
  return m_ForceAjax;
}

const AString& AOSCommand::getInputProcessorName() const
{ 
  return m_InputProcessor;
}

const AString& AOSCommand::getOutputGeneratorName() const 
{ 
  return m_OutputGenerator;
}

AString& AOSCommand::useCommandName()
{
  return m_Command;
}
