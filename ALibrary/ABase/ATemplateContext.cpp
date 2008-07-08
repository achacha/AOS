/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ATemplateContext.hpp"

void ATemplateContext::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Objects={" << std::endl;
  m_Objects.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Model={" << std::endl;
  m_Model.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_EventVisitor={" << std::endl;
  m_EventVisitor.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplateContext::  ATemplateContext(ABasePtrContainer& objects, AXmlDocument& model, AEventVisitor& visitor) :
  m_Objects(objects),
  m_Model(model),
  m_EventVisitor(visitor)
{
}

ATemplateContext::~ATemplateContext()
{
}

ABasePtrContainer& ATemplateContext::useObjects()
{
  return m_Objects;
}

AXmlDocument& ATemplateContext::useModel()
{
  return m_Model;
}

AEventVisitor& ATemplateContext::useEventVisitor()
{
  return m_EventVisitor;
}
