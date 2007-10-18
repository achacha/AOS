#include "pchAOS_Base.hpp"
#include "AOSModuleInfo.hpp"

#ifdef __DEBUG_DUMP__
void AOSModuleInfo::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModules @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Class=" << m_Class << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m_ModuleParams={" << std::endl;
  m_ModuleParams.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
 
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSModuleInfo::AOSModuleInfo(const AString& className, const AXmlElement& paramsBase) :
  m_Class(className),
  m_ModuleParams(ASW("params",6))
{
  paramsBase.emitXmlContent(m_ModuleParams);
}

const AString& AOSModuleInfo::getModuleClass() const
{
  return m_Class;
}

AXmlElement& AOSModuleInfo::useParams()
{
  return m_ModuleParams;
}

const AXmlElement& AOSModuleInfo::getParams() const
{
  return m_ModuleParams;
}
