#ifndef INCLUDED__AOSAdminCommandRegistry_HPP__
#define INCLUDED__AOSAdminCommandRegistry_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"

class AOSAdminCommandInterface;
class AOSServices;

class AOS_BASE_API AOSAdminCommandRegistry : public ADebugDumpable
{
public:
  AOSAdminCommandRegistry(AOSServices&);
  ~AOSAdminCommandRegistry();

  /*!
  Insert/remove an object reference by name
  */
  void insert(AOSAdminCommandInterface *object);
  
  /*!
  Get registered admin command object ptr
  
  Returns NULL if not found
  */
  AOSAdminCommandInterface *get(const AString& name) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AOSServices& m_Services;
  
  typedef std::map<AString, AOSAdminCommandInterface *> CONTAINER;
  CONTAINER m_AdminCommands;
};

#endif // INCLUDED__AOSAdminCommandRegistry_HPP__
