/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_TestPass_HPP__
#define INCLUDED__AOSModule_TestPass_HPP__

#include "apiAOS_Test.hpp"

class AOS_TEST_API AOSModule_TestPass : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_TestPass(AOSServices&);

  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_TestPass_HPP__
