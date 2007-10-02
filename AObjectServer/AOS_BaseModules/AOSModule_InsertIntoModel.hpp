#ifndef INCLUDED__AOSModule_InsertIntoModel_HPP__
#define INCLUDED__AOSModule_InsertIntoModel_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_InsertIntoModel : public AOSModuleInterface
{
public:
  AOSModule_InsertIntoModel(ALog&);
  virtual bool execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_InsertIntoModel_HPP__
