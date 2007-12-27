#ifndef INCLUDED__AOSModule_PublishInput_HPP__
#define INCLUDED__AOSModule_PublishInput_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_PublishInput : public AOSModuleInterface
{
public:
  AOSModule_PublishInput(AOSServices&);

  /*!
  Execute module
  */
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_PublishInput_HPP__
