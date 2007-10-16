#include "pchAOS_Base.hpp"
#include "AOSModuleInterface.hpp"

#ifdef __DEBUG_DUMP__
void AOSModuleInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModuleInterface @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Objects={";
  m_Objects.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m_ExecutionTimeAverage=" << std::endl;
  m_ExecutionTimeAverage.debugDump(os, indent+2);
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSModuleInterface::AOSModuleInterface(AOSServices& services) :
  m_Services(services),
  m_Objects("module")
{
}

void AOSModuleInterface::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

  addProperty(eBase, ASW("ExecutionTimeAverage",20), m_ExecutionTimeAverage);
  addProperty(eBase, ASW("ExecutionTimeCount",18), AString::fromU8(m_ExecutionTimeAverage.getCount()));
}
