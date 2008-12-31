/*
Written by Alex Chachanashvili

$Id: AOSAdminRegistry.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommandRegistry.hpp"
#include "AOSServices.hpp"
#include "AOSAdminCommandInterface.hpp"
#include "AException.hpp"

void AOSAdminCommandRegistry::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_AdminCommands={" << std::endl;
  for (CONTAINER::const_iterator cit = m_AdminCommands.begin(); cit != m_AdminCommands.end(); ++cit)
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "=" << typeid(*(*cit).second).name() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSAdminCommandRegistry::AOSAdminCommandRegistry(AOSServices& services) :
  m_Services(services)
{
}

AOSAdminCommandRegistry::~AOSAdminCommandRegistry()
{
  for (CONTAINER::iterator it = m_AdminCommands.begin(); it != m_AdminCommands.end(); ++it)
  {
    delete (*it).second;
  }
}

void AOSAdminCommandRegistry::insert(AOSAdminCommandInterface *pCommand)
{
  if (!pCommand || pCommand->getName().isEmpty())
    ATHROW(NULL, AException::InvalidParameter);

  if (m_AdminCommands.end() != m_AdminCommands.find(pCommand->getName()))
  {
    m_Services.useLog().add(ASWNL("AOSAdminRegistry::insert"), ASWNL("Duplicate admin command detected, ignoring"), pCommand->getName(), ALog::EVENT_WARNING);
  }

  //a_Insert/replace
  m_AdminCommands[pCommand->getName()] = pCommand;
}

AOSAdminCommandInterface* AOSAdminCommandRegistry::get(const AString& commandName) const
{
  CONTAINER::const_iterator cit = m_AdminCommands.find(commandName);
  if (cit != m_AdminCommands.end())
    return ((*cit).second);
  else
    return NULL;
}

