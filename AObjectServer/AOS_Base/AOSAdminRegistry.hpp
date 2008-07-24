#ifndef INCLUDED__AOSAdminRegistry_HPP__
#define INCLUDED__AOSAdminRegistry_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"

class ALog;
class AOSAdminInterface;

class AOS_BASE_API AOSAdminRegistry : public ADebugDumpable
{
public:
  AOSAdminRegistry(ALog&);
  virtual ~AOSAdminRegistry();

  /*!
  Insert/remove an object reference by name
  **/
  void insert(const AString& name, AOSAdminInterface& object);
  void remove(const AString& name);
  
  /*!
  Get registered admin object ptr
  Returns NULL if not found
  */
  AOSAdminInterface *getAdminObject(const AString& name) const;

  /*!
  listAdminObjects appends to the existing list, returns items appended
  **/
  u4 listAdminObjects(LIST_AString&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  ALog& m_Log;
  
  typedef std::map<AString, AOSAdminInterface *> CONTAINER;
  CONTAINER m_AdminObjects;
};

#endif // INCLUDED__AOSAdminRegistry_HPP__
