/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSModules.hpp"
#include "AOSModuleInfo.hpp"

void AOSModules::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModules @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Modules={" << std::endl;
  for (LIST_AOSMODULE_PTRS::const_iterator cit = m_Modules.begin(); cit != m_Modules.end(); ++cit)
    (*cit)->debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
 
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSModules::AOSModules()
{
}

AOSModules::~AOSModules()
{
  try
  {
    for (LIST_AOSMODULE_PTRS::iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
      delete (*it);
  }
  catch(...) {}
}

AOSModules::LIST_AOSMODULE_PTRS& AOSModules::use()
{
  return m_Modules;
}

const AOSModules::LIST_AOSMODULE_PTRS& AOSModules::get() const
{
  return m_Modules;
}
