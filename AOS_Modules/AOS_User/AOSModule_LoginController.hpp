#ifndef INCLUDED__AOSModule_LoginController_HPP__
#define INCLUDED__AOSModule_LoginController_HPP__

#include "apiAOS_User.hpp"

/*!
AuthenticateUser
*/
class AOS_USER_API AOSModule_LoginController : public AOSModuleInterface
{
public:
  AOSModule_LoginController(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_LoginController_HPP__
