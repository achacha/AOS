/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_InsertIntoModel_HPP__
#define INCLUDED__AOSModule_InsertIntoModel_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Insert XML directly into the model
*/
class AOS_BASEMODULES_API AOSModule_InsertIntoModel : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_InsertIntoModel(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_InsertIntoModel_HPP__
