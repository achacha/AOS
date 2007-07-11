#ifndef INCLUDED__AOSModule_AlterContext_HPP__
#define INCLUDED__AOSModule_AlterContext_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_AlterContext : public AOSModuleInterface
{
public:
  AOSModule_AlterContext(ALog&);

  virtual bool execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_AlterContext_HPP__
