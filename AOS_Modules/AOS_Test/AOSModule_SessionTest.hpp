/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_SessionTest_HPP__
#define INCLUDED__AOSModule_SessionTest_HPP__

#include "apiAOS_Test.hpp"

class AOS_TEST_API AOSModule_SessionTest : public AOSModuleInterface
{
public:
  AOSModule_SessionTest(AOSServices&);

  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;

private:
  static const AString S_COUNT;
  static const AString S_OUTPATH;

};

#endif //INCLUDED__AOSModule_SessionTest_HPP__
