#include "pchAOS_User.hpp"
#include "AOSModule_AuthenticateUser.hpp"

const AString& AOSModule_AuthenticateUser::getClass() const
{
  static const AString CLASS("AuthenticateUser");
  return CLASS;
}

AOSModule_AuthenticateUser::AOSModule_AuthenticateUser(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_AuthenticateUser::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  context.useSessionData().useData().overwriteElement(AOS_User_Constants::SESSION_ISLOGGEDIN);

  AString str;
  if (context.useSessionData().useData().emitString(AOS_User_Constants::SESSION_REDIRECTURL, str))
  {
    context.setResponseRedirect(str);
    context.useSessionData().useData().remove(AOS_User_Constants::SESSION_REDIRECTURL);

    return AOSContext::RETURN_REDIRECT;
  }


  return AOSContext::RETURN_OK;
}
