/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSModule_classified_input_HPP__
#define INCLUDED__AOSModule_classified_input_HPP__

#include "apiAOS_Classified.hpp"

class AOS_CLASSIFIED_API AOSModule_classified_input : public AOSModuleInterface
{
public:
  AOSModule_classified_input(AOSServices&);
  
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_classified_input_HPP__
