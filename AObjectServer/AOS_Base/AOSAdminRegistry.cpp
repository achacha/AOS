/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminRegistry.hpp"
#include "AException.hpp"
#include "ALog.hpp"

void AOSAdminRegistry::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_AdminObjects={" << std::endl;
  for (CONTAINER::const_iterator cit = m_AdminObjects.begin(); cit != m_AdminObjects.end(); ++cit)
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "=" << typeid(*(*cit).second).name() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSAdminRegistry::AOSAdminRegistry(ALog& aLog) :
  m_Log(aLog)
{
}

AOSAdminRegistry::~AOSAdminRegistry()
{
}

void AOSAdminRegistry::insert(const AString& name, AOSAdminInterface& object)
{
  if (name.isEmpty())
    ATHROW(this, AException::InvalidParameter);


  if (m_AdminObjects.find(name) != m_AdminObjects.end())
    m_Log.add(ASWNL("Admin object already registered"), name, ALog::EVENT_WARNING);

  //a_Insert/replace
  m_AdminObjects[name] = &object;
}

void AOSAdminRegistry::remove(const AString& name)
{
  m_AdminObjects.erase(name);
}

AOSAdminInterface* AOSAdminRegistry::getAdminObject(const AString& name) const
{
  CONTAINER::const_iterator cit = m_AdminObjects.find(name);
  if (cit != m_AdminObjects.end())
    return ((*cit).second);
  else
    return NULL;
}

u4 AOSAdminRegistry::listAdminObjects(LIST_AString& result) const
{
  u4 count = 0;
  CONTAINER::const_iterator cit = m_AdminObjects.begin();
  while(cit != m_AdminObjects.end())
  {
    result.push_back((*cit).first);
    ++cit;
    ++count;
  }
  return count;
}

