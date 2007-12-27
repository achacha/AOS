#ifndef INCLUDED__AOSModule_AuthenticateUser_HPP__
#define INCLUDED__AOSModule_AuthenticateUser_HPP__

#include "apiAOS_User.hpp"

/*!
AuthenticateUser
*/
class AOS_USER_API AOSModule_AuthenticateUser : public AOSModuleInterface
{
public:
  AOSModule_AuthenticateUser(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_AuthenticateUser_HPP__
