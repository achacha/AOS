#ifndef INCLUDED__AOSModule_SAMPLEMODULE_HPP__
#define INCLUDED__AOSModule_SAMPLEMODULE_HPP__

#include "apiAOS_SAMPLEPROJECT.hpp"

/*!
SAMPLEMODULE
*/
class AOS_SAMPLEPROJECTUPPERCASE_API AOSModule_SAMPLEMODULE : public AOSModuleInterface
{
public:
  /*!
  Class name
  Used to access this instance of the module
  Also used in registering it with administration
  MUST BE unique across all modules, input processors, output generators and all internal classes (which start with AOS)
  */
  static const AString CLASS;

public:
  /*!
  ctor
  
  @param services AOSServices object stored as reference by base class
  */
  AOSModule_SAMPLEMODULE(AOSServices& services);
  
  /*!
  Execute
  
  @param context AOSContext for the request
  @param moduleParams Parameters for the module as defined in the controller XML
  @return Result of the execution
  */
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface

  @return Name of this module
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_SAMPLEMODULE_HPP__
