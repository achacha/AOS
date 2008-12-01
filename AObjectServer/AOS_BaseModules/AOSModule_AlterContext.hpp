/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_AlterContext_HPP__
#define INCLUDED__AOSModule_AlterContext_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
AlterContext
*/
class AOS_BASEMODULES_API AOSModule_AlterContext : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_AlterContext(AOSServices&);

  /*!
  Execute module
  */
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;

private:
  //Set a cookie from element
  void _processSetCookie(ACookies&, AXmlElement::CONST_CONTAINER&);
};

#endif //INCLUDED__AOSModule_AlterContext_HPP__
