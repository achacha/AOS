/*
Written by Alex Chachanashvili

$Id: AOSModule_SetSessionLocale.hpp 277 2008-09-11 22:06:45Z achacha $
*/
#ifndef INCLUDED__AOSModule_SetSessionLocale_HPP__
#define INCLUDED__AOSModule_SetSessionLocale_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
AlterContext
*/
class AOS_BASEMODULES_API AOSModule_SetSessionLocale : public AOSModuleInterface
{
public:
  AOSModule_SetSessionLocale(AOSServices&);

  /*!
  Execute module
  */
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_SetSessionLocale_HPP__
