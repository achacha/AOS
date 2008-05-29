/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_User.hpp"
#include "AOSModule_User_Logout.hpp"
#include "AOSUser.hpp"

const AString& AOSModule_User_Logout::getClass() const
{
  static const AString CLASS("User.Logout");
  return CLASS;
}

AOSModule_User_Logout::AOSModule_User_Logout(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_User_Logout::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Logout requested, flag as logged out
  AXmlElement *pUser = context.useSessionData().useData().findElement(AOS_User_Constants::SESSION_USER);
  if (pUser)
  {
    pUser->remove(AOSUser::IS_LOGGED_IN);
  }
  else
  {
    //a_Not logged in
  }
  
  return AOSContext::RETURN_OK;
}
