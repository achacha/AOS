/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_classified_submit_HPP__
#define INCLUDED__AOSModule_classified_submit_HPP__

#include "apiAOS_Classified.hpp"

class AOS_CLASSIFIED_API AOSModule_classified_submit : public AOSModuleInterface
{
public:
  AOSModule_classified_submit(AOSServices&);
  
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_classified_submit_HPP__
