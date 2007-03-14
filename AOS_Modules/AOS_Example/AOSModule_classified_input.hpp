#ifndef INCLUDED__AOSModule_classified_input_HPP__
#define INCLUDED__AOSModule_classified_input_HPP__

#include "apiAOS_Example.hpp"

class AOS_EXAMPLE_API AOSModule_classified_input : public AOSModuleInterface
{
public:
  AOSModule_classified_input(ALog&);
  virtual bool execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_classified_input_HPP__
