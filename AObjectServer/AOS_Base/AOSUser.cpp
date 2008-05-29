/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSUser.hpp"

const AString AOSUser::USERNAME("username");
const AString AOSUser::IS_LOGGED_IN("isLoggedIn");

void AOSUser::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Element={" << std::endl;
  m_Element.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSUser::AOSUser(AXmlElement& element) :
  m_Element(element)
{
}

AXmlElement& AOSUser::useElement()
{
  return m_Element;
}

const AXmlElement& AOSUser::getElement() const
{
  return m_Element;
}
