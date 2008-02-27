
#include "pchAOS_Base.hpp"
#include "AOSCommand.hpp"
#include "AXmlDocument.hpp"
#include "ALog.hpp"
#include "AOSAdminRegistry.hpp"
#include "AOSModules.hpp"

const AString AOSCommand::S_COMMAND("command",7);
const AString AOSCommand::S_INPUT("input",5);
const AString AOSCommand::S_MODULE("module",6);
const AString AOSCommand::S_OUTPUT("output",6);

const AString AOSCommand::S_CLASS("class",5);
const AString AOSCommand::S_ENABLED("enabled",7);
const AString AOSCommand::S_AJAX("ajax",4);
const AString AOSCommand::S_ALIAS("alias",5);
const AString AOSCommand::S_GZIP("gzip",4);

void AOSCommand::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSCommand @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_CommandPath=" << m_CommandPath << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Enabled=" << (m_Enabled ? "true" : "false") << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ForceAjax=" << (m_ForceAjax ? "true" : "false") << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_GZipLevel=" << m_GZipLevel << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_CommandAlias=" << m_CommandAlias << std::endl;

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
 
  AOSAdminInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSCommand::AOSCommand(const AString& path, ALog& log) :
  m_CommandPath(path),
  m_InputParams(S_INPUT),
  m_OutputParams(S_OUTPUT),
  m_Enabled(true),
  m_ForceAjax(false),
  m_GZipLevel(0),
  m_Log(log)
{
}

AOSCommand::~AOSCommand()
{
}

const AString& AOSCommand::getClass() const
{
  static const AString CLASS("AOSCommand");
  return CLASS;
}

void AOSCommand::adminEmitXml(AXmlElement& thisRoot, const AHTTPRequestHeader& request)
{
  //a_Display the commadn name (which is the path)
  thisRoot.addAttribute(ASW("display",7), m_CommandPath);

  //a_Alias if any
  thisRoot.addAttribute(S_ALIAS, m_CommandAlias);

  ARope rope;
  if (!m_InputProcessor.isEmpty())
  {
    adminAddProperty(thisRoot, S_INPUT, m_InputProcessor);
    m_InputParams.emit(rope, 0);
    adminAddProperty(thisRoot, S_INPUT+ASW(".params",7), rope, AXmlElement::ENC_CDATASAFE);
  }

  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.use().begin();
  int i=0;
  while (cit != m_Modules.use().end())
  {
    ARope ropeName(S_MODULE);
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

  if (!m_OutputGenerator.isEmpty())
  {
    adminAddProperty(thisRoot, S_OUTPUT, m_OutputGenerator);
    rope.clear();
    m_OutputParams.emit(rope, 0);
    adminAddProperty(thisRoot, S_OUTPUT+ASW(".params",7), rope, AXmlElement::ENC_CDATASAFE);
  }

  adminAddPropertyWithAction(
    thisRoot, 
    S_ENABLED, 
    AString::fromBool(m_Enabled),
    ASW("Update",6), 
    ASWNL("Enable(1) or Disable(0) the command"),
    ASW("Set",3));
  
  adminAddPropertyWithAction(
    thisRoot, 
    S_AJAX, 
    AString::fromBool(m_ForceAjax),
    ASW("Update",6), 
    ASWNL("Force AJAX(1) or Default(0)"),
    ASW("Set",3)
  );

  adminAddPropertyWithAction(
    thisRoot, 
    S_GZIP, 
    AString::fromInt(m_GZipLevel),
    ASW("Update",6), 
    ASWNL("GZip compression level, (0)ff, (1)Minimum-(9)Maximum"),
    ASW("Set",3)
  );
}

void AOSCommand::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(S_ENABLED, str))
  {
    m_Enabled = (str.at(0) == '0' ? false : true);
  }
  else if (request.getUrl().getParameterPairs().get(S_AJAX, str))
  {
    m_ForceAjax = (str.at(0) == '0' ? false : true);
  }
  else if (request.getUrl().getParameterPairs().get(S_GZIP, str))
  {
    int level = str.toInt();
    if (level >=0 && level <=9)
      m_GZipLevel = level;
  }
}

AXmlElement& AOSCommand::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  if (!m_CommandAlias.isEmpty())
    thisRoot.addAttribute(S_ALIAS, m_CommandAlias);
  
  thisRoot.addAttribute(S_ENABLED, m_Enabled ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
  thisRoot.addAttribute(S_AJAX, m_ForceAjax ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
  thisRoot.addAttribute(S_GZIP, AString::fromInt(m_GZipLevel));
  
  //a_Input
  AXmlElement& eInput = thisRoot.addElement(S_INPUT);
  eInput.addAttribute(S_CLASS, m_InputProcessor);
  if (m_InputParams.hasElements()) 
    eInput.addContent(m_InputParams.clone());
  
  //a_Modules
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.get().begin();
  while (cit != m_Modules.get().end())
  {
    AXmlElement& eModule = thisRoot.addElement(S_MODULE);
    eModule.addAttribute(S_CLASS, (*cit)->getModuleClass());
    if ((*cit)->getParams().hasElements())
      eModule.addContent((*cit)->getParams().clone());
    ++cit;
  }

  //a_Output
  AXmlElement& eOutput = thisRoot.addElement(S_OUTPUT);
  eOutput.addAttribute(S_CLASS, m_OutputGenerator);
  if (m_OutputParams.hasElements())
    eOutput.addContent(m_OutputParams.clone());

  return thisRoot;
}

void AOSCommand::fromXml(const AXmlElement& element)
{
  AString str(256,128);

  //a_Set command alias
  element.getAttributes().get(S_ALIAS, m_CommandAlias);

  //a_Enabled?
  element.getAttributes().get(S_ENABLED, str);
  if (str.equals(AConstant::ASTRING_ZERO) || str.equals(AConstant::ASTRING_FALSE))
    m_Enabled = false;
  else
    m_Enabled = true;

  //a_Is Ajax call?
  str.clear();
  element.getAttributes().get(S_AJAX, str);
  if (str.equals(AConstant::ASTRING_ONE) || str.equals(AConstant::ASTRING_TRUE))
    m_ForceAjax = true;
  else
    m_ForceAjax = false;

  //a_Is GZip compressed?
  str.clear();
  element.getAttributes().get(S_GZIP, str);
  int level = str.toInt();
  if (level >= 0 && level <= 9)
    m_GZipLevel = level;
  else
    m_GZipLevel = 0;

  //a_Get input processor
  m_InputProcessor.clear();
  m_InputParams.clear();
  const AXmlElement *pNode = element.findElement(S_INPUT);
  if (pNode)
  {
    pNode->getAttributes().get(S_CLASS, m_InputProcessor);
    pNode->emitXmlContent(m_InputParams);
  }

  //a_Get output generator
  m_OutputGenerator.clear();
  m_OutputParams.clear();
  pNode = element.findElement(S_OUTPUT);
  if (pNode)
  {
    pNode->getAttributes().get(S_CLASS, m_OutputGenerator);
    pNode->emitXmlContent(m_OutputParams);
  }

  //a_Get module names
  m_Modules.use().clear();
  AXmlElement::CONST_CONTAINER nodes;
  element.find(S_MODULE, nodes);
  
  AString strClass;
  for (AXmlElement::CONST_CONTAINER::const_iterator citModule = nodes.begin(); citModule != nodes.end(); ++citModule)
  {
    //a_Get module 'class', this is the registered MODULE_CLASS and get module params
    if ((*citModule)->getAttributes().get(S_CLASS, strClass) && !strClass.isEmpty())
    {
      //a_Add new module info object
      m_Modules.use().push_back(new AOSModuleInfo(strClass, *(*citModule)));
    }
    strClass.clear();
  }
}

void AOSCommand::adminRegisterObject(AOSAdminRegistry& registry)
{
  AString str(getClass());
  str.append(':');
  str.append(m_CommandPath);
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

const AString& AOSCommand::getCommandAlias() const
{ 
  return m_CommandAlias;
}

const AString& AOSCommand::getCommandPath() const
{ 
  return m_CommandPath;
}

void AOSCommand::emitCommandBasePath(AOutputBuffer& target) const
{
  size_t pos = m_CommandPath.rfind('/');
  AASSERT(this, AConstant::npos != pos);       //a_command path must all be absolute from server perspective
  if (AConstant::npos != pos)
  {
    m_CommandPath.peek(target, 0, pos+1);
  }
}

bool AOSCommand::isEnabled() const
{
  return m_Enabled;
}

bool AOSCommand::isForceAjax() const
{
  return m_ForceAjax;
}

int AOSCommand::getGZipLevel() const
{
  return m_GZipLevel;
}

const AString& AOSCommand::getInputProcessorName() const
{ 
  return m_InputProcessor;
}

const AString& AOSCommand::getOutputGeneratorName() const 
{ 
  return m_OutputGenerator;
}
