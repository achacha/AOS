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
  Insert an object reference by name
  The objects refered to are NOT OWNED and will NOT be deleted

  @param name of the object
  @param object reference to register
  **/
  void insert(const AString& name, AOSAdminInterface& object);
  
  /*!
  Remove reference to an object
  
  @param name of the object
  */
  void remove(const AString& name);
  
  /*!
  Get registered admin object ptr
  
  @return NULL if not found, object pointer which should not be deleted
  */
  AOSAdminInterface *getAdminObject(const AString& name) const;

  /*!
  listAdminObjects appends to the existing list, returns items appended
  
  @param target for the list of admin object names (will append to the existing list)
  @retrurn number of names added to target
  **/
  u4 listAdminObjects(LIST_AString& target) const;

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
