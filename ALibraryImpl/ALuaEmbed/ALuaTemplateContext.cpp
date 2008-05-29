/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchALuaEmbed.hpp"
#include "ALuaTemplateContext.hpp"

void ALuaTemplateContext::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  if (mp_Lua)
  {
    ADebugDumpable::indent(os, indent+1) << "m_Lua={" << std::endl;
    mp_Lua->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_Lua=NULL" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_LibrariesToLoad=" << m_LibrariesToLoad << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ALuaTemplateContext::ALuaTemplateContext(
  ABasePtrContainer& objects, 
  AXmlDocument& model, 
  u4 maskLibrariesToLoad
) :
  ATemplateContext(objects, model),
  mp_Lua(NULL),
  m_LibrariesToLoad(maskLibrariesToLoad)
{
}

ALuaTemplateContext::~ALuaTemplateContext()
{
  try
  {
    delete mp_Lua;
  }
  catch(...) {}
}

void ALuaTemplateContext::addUserDefinedLibrary(ALuaEmbed::LUA_OPENLIBRARY_FPTR fptr)
{
  m_UserDefinedLibFunctions.push_back(fptr);
}

ALuaEmbed& ALuaTemplateContext::useLua()
{
  if (!mp_Lua)
  {
    mp_Lua = new ALuaEmbed(m_LibrariesToLoad);

    //a_Load queued up user defined libraries
    for (USER_DEFINED_FPTRS::iterator it = m_UserDefinedLibFunctions.begin(); it != m_UserDefinedLibFunctions.end(); ++it)
      mp_Lua->loadUserLibrary(*it);
  }

  return *mp_Lua;
}
