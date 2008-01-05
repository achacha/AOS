#include "pchAOS_User.hpp"
#include "AOSModule_AuthenticateUser.hpp"

const AString& AOSModule_AuthenticateUser::getClass() const
{
  static const AString CLASS("User.Authenticate");
  return CLASS;
}

AOSModule_AuthenticateUser::AOSModule_AuthenticateUser(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_AuthenticateUser::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //
  //TODO: This is where the check will go, for now just set the LoggedIn regardless of input
  //
  AString password;
  bool isValidUser = context.useRequestParameterPairs().get(AOS_User_Constants::PASSWORD, password);
  isValidUser &= password.equals(ASWNL("60c6d277a8bd81de7fdde19201bf9c58a3df08f4"));
  if (isValidUser)
  {
    context.useSessionData().useData().remove(AOS_User_Constants::SESSION_LOGINFAILCOUNT);
    context.useSessionData().useData().overwriteElement(AOS_User_Constants::SESSION_ISLOGGEDIN);

    AString str;
    if (context.useSessionData().useData().emitString(AOS_User_Constants::SESSION_REDIRECTURL, str))
    {
      context.setResponseRedirect(str);
      context.useSessionData().useData().remove(AOS_User_Constants::SESSION_REDIRECTURL);

      return AOSContext::RETURN_REDIRECT;
    }
  }
  else
  {
    AString str;
    if (context.useSessionData().useData().emitContentFromPath(AOS_User_Constants::SESSION_LOGINFAILCOUNT, str))
    {
      //a_Increment failure
      AString attempts;
      if (moduleParams.emitContentFromPath(AOS_User_Constants::PARAM_ATTEMPTS, attempts))
      {
        //a_Limit on attempts
        if (attempts.toInt() < str.toInt())
        {
          //a_Increment count
          context.useSessionData().useData().setInt(AOS_User_Constants::SESSION_LOGINFAILCOUNT, attempts.toInt()+1);

          //a_Try again
          str.clear();
          if (moduleParams.emitContentFromPath(AOS_User_Constants::PARAM_REDIRECT_LOGINPAGE, str))
          {
            context.setResponseRedirect(str);
            return AOSContext::RETURN_REDIRECT;
          }
          else
          {
            context.addError(getClass(), ARope("Missing module parameter for redirect on login failure: ")+AOS_User_Constants::PARAM_REDIRECT_FAILURE);
            return AOSContext::RETURN_ERROR;
          }
        }
        else
        {
          context.setExecutionState(ARope("Login failure #")+attempts);
        }
      }
    }
    else
    {
      //a_First failure
      context.useSessionData().useData().setInt(AOS_User_Constants::SESSION_LOGINFAILCOUNT, 1);
    }

      
    //a_Login failed
    str.clear();
    if (moduleParams.emitContentFromPath(AOS_User_Constants::PARAM_REDIRECT_FAILURE, str))
    {
      context.setResponseRedirect(str);
      return AOSContext::RETURN_REDIRECT;
    }
    else
    {
      context.addError(getClass(), ARope("Missing module parameter for redirect on login failure: ")+AOS_User_Constants::PARAM_REDIRECT_FAILURE);
      return AOSContext::RETURN_ERROR;
    }

  }


  return AOSContext::RETURN_OK;
}
