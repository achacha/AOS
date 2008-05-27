/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSModuleInfo.hpp"
#include "AOSContext.hpp"

AString AOSModuleInfo::COND_IF("if",2);
AString AOSModuleInfo::COND_IF_NOT("if-not",6);

void AOSModuleInfo::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModules @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Class=" << m_Class << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m_ModuleParams={" << std::endl;
  m_ModuleParams.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
 
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSModuleInfo::AOSModuleInfo(const AString& className, const AXmlElement& paramsBase) :
  m_Class(className),
  m_ModuleParams(ASW("params",6))
{
  paramsBase.emitXmlContent(m_ModuleParams);
  paramsBase.getAttributes().get(COND_IF, m_If);
  paramsBase.getAttributes().get(COND_IF_NOT, m_IfNot);
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

bool AOSModuleInfo::isExecute(AOSContext& context)
{
  if (m_If.getSize() > 0)
  {
    if (context.useModel().exists(m_If))
    {
      if (m_IfNot.getSize() > 0)
        return (context.useModel().exists(m_IfNot) ? false : true);
      else
        return true;
    }
    else
      return false;
  }
  else if (m_IfNot.getSize() > 0)
  {
    return (context.useModel().exists(m_If) ? false : true);
  }
  else
    return true;  //a_No if or if-not detected
}
