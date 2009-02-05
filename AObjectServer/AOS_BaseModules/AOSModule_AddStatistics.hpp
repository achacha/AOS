/*
Written by Alex Chachanashvili

$Id: AOSModule_AddStatistics.hpp 315 2008-12-01 14:05:00Z achacha $
*/
#ifndef INCLUDED__AOSModule_AddStatistics_HPP__
#define INCLUDED__AOSModule_AddStatistics_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_AddStatistics : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_AddStatistics(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_AddStatistics_HPP__
