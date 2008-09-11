/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_Template_HPP__
#define INCLUDED__AOSModule_Template_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Template module
Loads and evaluates a template and optionally writes output to an element
*/
class AOS_BASEMODULES_API AOSModule_Template : public AOSModuleInterface
{
public:
  AOSModule_Template(AOSServices&);

  /*!
  Execute a template
  */
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_Template_HPP__
