/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_User_Authenticate_HPP__
#define INCLUDED__AOSModule_User_Authenticate_HPP__

#include "apiAOS_User.hpp"

/*!
AuthenticateUser

Reference to URL redirect:
<module>
  <redirect>
    <retry reference="session|model">path to get the url</retry>
    <failure/>
  </redirect>
</module>

Imemdiate URL redirect:
<module>
  <redirect>
    <retry>url</retry>
    <failure/>
  </redirect>
</module>

*/
class AOS_USER_API AOSModule_User_Authenticate : public AOSModuleInterface
{
public:
  AOSModule_User_Authenticate(AOSServices&);
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif // INCLUDED__AOSModule_User_Authenticate_HPP__
