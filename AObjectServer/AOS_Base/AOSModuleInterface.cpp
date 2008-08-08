/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSModuleInterface.hpp"

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

AOSModuleInterface::AOSModuleInterface(AOSServices& services) :
  m_Services(services)
{
}

AOSModuleInterface::~AOSModuleInterface()
{
}

void AOSModuleInterface::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  adminAddProperty(eBase, ASW("ExecutionTimeAverage",20), m_ExecutionTimeAverage);
  adminAddProperty(eBase, ASW("ExecutionTimeCount",18), AString::fromU8(m_ExecutionTimeAverage.getCount()));

  adminAddProperty(eBase, ASW("Objects",7), m_Objects);
}
