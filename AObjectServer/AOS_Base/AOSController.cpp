/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSController.hpp"
#include "ALog.hpp"
#include "AOSAdminRegistry.hpp"
#include "AOSModules.hpp"

const AString AOSController::S_CONTROLLER("controller",10);
const AString AOSController::S_INPUT("input",5);
const AString AOSController::S_MODULE("module",6);
const AString AOSController::S_OUTPUT("output",6);

const AString AOSController::S_CLASS("class",5);
const AString AOSController::S_ENABLED("enabled",7);
const AString AOSController::S_AJAX("ajax",4);
const AString AOSController::S_ALIAS("alias",5);
const AString AOSController::S_GZIP("gzip",4);
const AString AOSController::S_CACHECONTROLNOCACHE("nocache",7);

void AOSController::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSController @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Path=" << m_Path << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Enabled=" << AString::fromBool(m_Enabled) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ForceAjax=" << AString::fromBool(m_ForceAjax) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_CacheControlNoCache=" << AString::fromBool(m_CacheControlNoCache) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_GZipLevel=" << m_GZipLevel << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Alias=" << m_Alias << std::endl;

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

AOSController::AOSController(const AString& path, ALog& log) :
  m_Path(path),
  m_InputParams(S_INPUT),
  m_OutputParams(S_OUTPUT),
  m_Enabled(true),
  m_ForceAjax(false),
  m_CacheControlNoCache(true),
  m_GZipLevel(0),
  m_Log(log)
{
}

AOSController::~AOSController()
{
}

const AString& AOSController::getClass() const
{
  static const AString CLASS("AOSController");
  return CLASS;
}

void AOSController::adminEmitXml(AXmlElement& thisRoot, const AHTTPRequestHeader& request)
{
  //a_Display the commadn name (which is the path)
  thisRoot.addAttribute(ASW("display",7), m_Path);

  //a_Alias if any
  thisRoot.addAttribute(S_ALIAS, m_Alias);

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
    S_CACHECONTROLNOCACHE, 
    AString::fromBool(m_CacheControlNoCache),
    ASW("Update",6), 
    ASWNL("'Cache-Control: no-cache'(1) or Allow caching by the browser(0)"),
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

void AOSController::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("property",8), str))
  {
    AString value;
    if (request.getUrl().getParameterPairs().get(ASW("Set",3), value))
    {
      //a_Toggle cache state and clear when disabled
      if (str.endsWith(S_ENABLED))
      {
        m_Enabled = value.toBool();
      }
      else if (str.endsWith(S_AJAX))
      {
        m_ForceAjax = value.toBool();
      }
      else if (str.endsWith(S_CACHECONTROLNOCACHE))
      {
        m_CacheControlNoCache = value.toBool();
      }
      else if (str.endsWith(S_GZIP))
      {
        int level = value.toInt();
        if (level >=0 && level <=9)
          m_GZipLevel = level;
      }
      else
        adminAddError(eBase, ARope("Unknown property: ")+str);
    }
    else
      adminAddError(eBase, ARope("Set value not found: ")+value);
  }
  else
    adminAddError(eBase, ARope("Property not found: ")+str);
}

AXmlElement& AOSController::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  if (!m_Alias.isEmpty())
    thisRoot.addAttribute(S_ALIAS, m_Alias);
  
  thisRoot.addAttribute(S_ENABLED, m_Enabled ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
  thisRoot.addAttribute(S_AJAX, m_ForceAjax ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
  thisRoot.addAttribute(S_CACHECONTROLNOCACHE, m_CacheControlNoCache ? AConstant::ASTRING_ONE : AConstant::ASTRING_ZERO);
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

void AOSController::fromXml(const AXmlElement& element)
{
  AString str(256,128);

  //a_Set command alias
  element.getAttributes().get(S_ALIAS, m_Alias);

  //a_Enabled?
  if (element.getAttributes().get(S_ENABLED, str))
    m_Enabled = str.toBool();

  //a_Is Ajax call?
  str.clear();
  if (element.getAttributes().get(S_AJAX, str))
    m_ForceAjax = str.toBool();

  //a_Is caching command call?
  str.clear();
  if (element.getAttributes().get(S_CACHECONTROLNOCACHE, str))
    m_CacheControlNoCache = str.toBool();

  //a_Is GZip compressed?
  str.clear();
  if (element.getAttributes().get(S_GZIP, str))
  {
    int level = str.toInt();
    if (level >= 0 && level <= 9)
      m_GZipLevel = level;
    else
      m_GZipLevel = 0;
  }

  //a_Get input processor
  m_InputProcessor.clear();
  m_InputParams.clear();
  const AXmlElement *pNode = element.findElement(S_INPUT);
  if (pNode)
  {
    if (pNode->getAttributes().get(S_CLASS, m_InputProcessor))
      pNode->emitXmlContent(m_InputParams);
  }

  //a_Get output generator
  m_OutputGenerator.clear();
  m_OutputParams.clear();
  pNode = element.findElement(S_OUTPUT);
  if (pNode)
  {
    if (pNode->getAttributes().get(S_CLASS, m_OutputGenerator))
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
      m_Modules.use().push_back(new AOSModuleInfo(strClass, *(*citModule)));
    
    strClass.clear();
  }
}

void AOSController::adminRegisterObject(AOSAdminRegistry& registry)
{
  AString str(getClass());
  str.append(':');
  str.append(m_Path);
  registry.insert(str, *this);
}

const AOSModules& AOSController::getModules() const
{
  return m_Modules;
}

const AXmlElement& AOSController::getInputParams() const
{
  return m_InputParams;
}

const AXmlElement& AOSController::getOutputParams() const
{
  return m_OutputParams;
}

const AString& AOSController::getAlias() const
{ 
  return m_Alias;
}

const AString& AOSController::getPath() const
{ 
  return m_Path;
}

void AOSController::emitBasePath(AOutputBuffer& target) const
{
  size_t pos = m_Path.rfind('/');
  AASSERT(this, AConstant::npos != pos);       //a_command path must all be absolute from server perspective
  if (AConstant::npos != pos)
  {
    m_Path.peek(target, 0, pos+1);
  }
}

bool AOSController::isEnabled() const
{
  return m_Enabled;
}

bool AOSController::isForceAjax() const
{
  return m_ForceAjax;
}

bool AOSController::isCacheControlNoCache() const
{
  return m_CacheControlNoCache;
}

int AOSController::getGZipLevel() const
{
  return m_GZipLevel;
}

const AString& AOSController::getInputProcessorName() const
{ 
  return m_InputProcessor;
}

const AString& AOSController::getOutputGeneratorName() const 
{ 
  return m_OutputGenerator;
}
