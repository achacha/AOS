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
  AString strSuccessPath;
  const AXmlElement *pSuccess = moduleParams.findElement(AOS_User_Constants::PARAM_REDIRECT_RETRYPAGE);
  if (!pSuccess)
  {
    context.addError(getClass(), ARope("Missing module parameter for redirect on retry: ")+AOS_User_Constants::PARAM_REDIRECT_FAILURE);
    return AOSContext::RETURN_ERROR;
  }
  else
  {
    AString str;
    if (pSuccess->getAttributes().get(AOS_User_Constants::PARAM_REDIRECT_LOGINPAGE_RELATIVEATTR, str))
    {
      str.clear();
      pSuccess->emitContent(str);
      if (!context.useModel().emitContentFromPath(str, strSuccessPath))
      {
        context.useEventVisitor().startEvent(ARope("AOSModule_AuthenticateUser: Missing model parameter for redirect: ")+str, AEventVisitor::EL_WARN);
      }
    }
    else
    {
      //a_Path in content
      pSuccess->emitContent(strSuccessPath);
    }
  }

  AString strFailurePath;
  if (!moduleParams.emitContentFromPath(AOS_User_Constants::PARAM_REDIRECT_FAILURE, strFailurePath))
  {
    context.addError(getClass(), ARope("Missing module parameter for redirect on login failure: ")+AOS_User_Constants::PARAM_REDIRECT_FAILURE);
    return AOSContext::RETURN_ERROR;
  }

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

    context.setResponseRedirect(strSuccessPath);
    context.useSessionData().useData().remove(AOS_User_Constants::SESSION_REDIRECTURL);
    return AOSContext::RETURN_REDIRECT;
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
          context.useEventVisitor().startEvent(ARope("Login attempt #")+attempts);
          context.useSessionData().useData().setInt(AOS_User_Constants::SESSION_LOGINFAILCOUNT, attempts.toInt()+1);

          //a_Try again
          context.setResponseRedirect(strFailurePath);
          return AOSContext::RETURN_REDIRECT;
        }
        else
        {
          context.useEventVisitor().startEvent(ARope("Login failure #")+attempts);
          context.setResponseRedirect(strFailurePath);
          return AOSContext::RETURN_REDIRECT;
        }
      }
    }
    else
    {
      //a_First failure
      context.useSessionData().useData().setInt(AOS_User_Constants::SESSION_LOGINFAILCOUNT, 1);
      context.setResponseRedirect(strFailurePath);
      return AOSContext::RETURN_REDIRECT;
    }
  }
  
  return AOSContext::RETURN_OK;
}
