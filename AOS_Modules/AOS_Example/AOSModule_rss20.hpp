/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_rss20_HPP__
#define INCLUDED__AOSModule_rss20_HPP__

#include "apiAOS_Example.hpp"

class AOS_EXAMPLE_API AOSModule_rss20 : public AOSModuleInterface
{
public:
  AOSModule_rss20(AOSServices&);
  
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_rss20_HPP__
