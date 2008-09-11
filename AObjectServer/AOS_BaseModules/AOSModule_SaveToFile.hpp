/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_SaveToFile_HPP__
#define INCLUDED__AOSModule_SaveToFile_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Save model element to file relative to the aos_root/data directory
*/
class AOS_BASEMODULES_API AOSModule_SaveToFile : public AOSModuleInterface
{
public:
  AOSModule_SaveToFile(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_SaveToFile_HPP__
