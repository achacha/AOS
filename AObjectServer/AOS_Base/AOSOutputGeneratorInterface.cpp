#include "pchAOS_Base.hpp"
#include "AOSOutputGeneratorInterface.hpp"

#ifdef __DEBUG_DUMP__
void AOSOutputGeneratorInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSOutputGeneratorInterface @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_ExecutionTimeAverage=" << std::endl;
  m_ExecutionTimeAverage.debugDump(os, indent+2);
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSOutputGeneratorInterface::AOSOutputGeneratorInterface(ALog& log) :
  m_Log(log)
{
}

void AOSOutputGeneratorInterface::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

  addProperty(eBase, ASW("ExecutionTimeAverage",20), m_ExecutionTimeAverage);
  addProperty(eBase, ASW("ExecutionTimeCount",18), AString::fromU8(m_ExecutionTimeAverage.getCount()));
}
