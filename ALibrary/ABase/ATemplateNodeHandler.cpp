/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "ATemplateNodeHandler.hpp"

#ifdef __DEBUG_DUMP__
void ATemplateNodeHandler::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNodeHandler::ATemplateNodeHandler()
{
}

ATemplateNodeHandler::~ATemplateNodeHandler()
{
}
