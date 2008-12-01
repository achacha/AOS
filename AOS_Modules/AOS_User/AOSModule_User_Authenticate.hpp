/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_User_Authenticate_HPP__
#define INCLUDED__AOSModule_User_Authenticate_HPP__

#include "apiAOS_User.hpp"

/*!
AuthenticateUser
*/
class AOS_USER_API AOSModule_User_Authenticate : public AOSModuleInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModule_User_Authenticate(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_User_Authenticate_HPP__
