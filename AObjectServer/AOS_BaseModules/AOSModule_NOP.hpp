/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_NOP_HPP__
#define INCLUDED__AOSModule_NOP_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_NOP : public AOSModuleInterface
{
public:
  AOSModule_NOP(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_NOP_HPP__
