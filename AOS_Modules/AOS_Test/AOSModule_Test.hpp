/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSModule_Test_HPP__
#define INCLUDED__AOSModule_Test_HPP__

#include "apiAOS_Test.hpp"

class AOS_TEST_API AOSModule_Test : public AOSModuleInterface
{
public:
  AOSModule_Test(AOSServices&);

  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_Test_HPP__
