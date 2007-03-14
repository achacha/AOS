#ifndef __AOSExecutorBase_HPP__
#define __AOSExecutorBase_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"

class AContext;
class ALog;
class AOSModuleInterface;

class AOS_BASE_API AOSExecutorBase : public ADebugDumpable
{
public:
  AOSExecutorBase(ALog&);
  virtual ~AOSExecutorBase();

  /*
   * Execute module specified in request header content-type field
  **/
  virtual void execute(AContext&) = 0;


protected:

};

#endif // __AOSExecutorBase_HPP__
