#ifndef INCLUDED__AOSModule_classified_submit_HPP__
#define INCLUDED__AOSModule_classified_submit_HPP__

#include "apiAOS_Example.hpp"

class AOS_EXAMPLE_API AOSModule_classified_submit : public AOSModuleInterface
{
public:
  AOSModule_classified_submit(AOSServices&);
  
  virtual bool execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_classified_submit_HPP__
