#ifndef INCLUDED__AOSModules_HPP__
#define INCLUDED__AOSModules_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"

class AOSModuleInfo;

/*!
List container for AOSModuleInfo pointers
Deletes them when done
*/
class AOS_BASE_API AOSModules : public ADebugDumpable
{
public:
  typedef std::list<AOSModuleInfo *> LIST_AOSMODULE_PTRS;

public:
  //! ctor
  AOSModules();
  
  //! dtor - delete pointers  
  virtual ~AOSModules();

  //! Access the container
  LIST_AOSMODULE_PTRS& use();

  //! Access the container
  const LIST_AOSMODULE_PTRS& get() const;

private:
  LIST_AOSMODULE_PTRS m_Modules;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSModules_HPP__
