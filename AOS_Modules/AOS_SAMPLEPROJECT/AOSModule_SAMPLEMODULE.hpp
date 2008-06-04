#ifndef INCLUDED__AOSModule_SAMPLEMODULE_HPP__
#define INCLUDED__AOSModule_SAMPLEMODULE_HPP__

#include "apiAOS_SAMPLEPROJECT.hpp"

/*!
SAMPLEMODULE
*/
class AOS_SAMPLEPROJECTUPPERCASE_API AOSModule_SAMPLEMODULE : public AOSModuleInterface
{
public:
  AOSModule_SAMPLEMODULE(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_SAMPLEMODULE_HPP__
