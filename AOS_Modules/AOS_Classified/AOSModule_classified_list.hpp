#ifndef INCLUDED__AOSModule_classified_list_HPP__
#define INCLUDED__AOSModule_classified_list_HPP__

#include "apiAOS_Classified.hpp"

class AOS_CLASSIFIED_API AOSModule_classified_list : public AOSModuleInterface
{
public:
  AOSModule_classified_list(AOSServices&);
  
  virtual bool execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_classified_list_HPP__
