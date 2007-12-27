#ifndef INCLUDED__AOSModule_TestFail_HPP__
#define INCLUDED__AOSModule_TestFail_HPP__

#include "apiAOS_Test.hpp"

class AOS_TEST_API AOSModule_TestFail : public AOSModuleInterface
{
public:
  AOSModule_TestFail(AOSServices&);
  \
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_TestFail_HPP__
