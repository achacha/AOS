#ifndef INCLUDED__AOSModule_ExecuteQuery_HPP__
#define INCLUDED__AOSModule_ExecuteQuery_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_ExecuteQuery : public AOSModuleInterface
{
public:
  AOSModule_ExecuteQuery(AOSServices&);
  virtual bool execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_ExecuteQuery_HPP__
