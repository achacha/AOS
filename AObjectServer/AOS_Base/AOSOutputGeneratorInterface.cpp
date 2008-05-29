/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSOutputGeneratorInterface.hpp"

void AOSOutputGeneratorInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSOutputGeneratorInterface @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_ExecutionTimeAverage=" << std::endl;
  m_ExecutionTimeAverage.debugDump(os, indent+2);
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSOutputGeneratorInterface::AOSOutputGeneratorInterface(AOSServices& services) :
  m_Services(services)
{
}

void AOSOutputGeneratorInterface::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  adminAddProperty(eBase, ASW("ExecutionTimeAverage",20), m_ExecutionTimeAverage);
  adminAddProperty(eBase, ASW("ExecutionTimeCount",18), AString::fromU8(m_ExecutionTimeAverage.getCount()));
}
