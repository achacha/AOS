/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_User_LoginController_HPP__
#define INCLUDED__AOSModule_User_LoginController_HPP__

#include "apiAOS_User.hpp"

/*!
AuthenticateUser
*/
class AOS_USER_API AOSModule_User_LoginController : public AOSModuleInterface
{
public:
  AOSModule_User_LoginController(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_User_LoginController_HPP__
