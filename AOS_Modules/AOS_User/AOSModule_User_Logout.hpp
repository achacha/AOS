/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_User_Logout_HPP__
#define INCLUDED__AOSModule_User_Logout_HPP__

#include "apiAOS_User.hpp"

/*!
User Logout
*/
class AOS_USER_API AOSModule_User_Logout : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_User_Logout(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_User_Logout_HPP__
