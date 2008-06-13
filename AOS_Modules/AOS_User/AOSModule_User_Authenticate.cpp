/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_User.hpp"
#include "AOSModule_User_Authenticate.hpp"
#include "AOSUser.hpp"
#include "AOSContextUtils.hpp"

const AString& AOSModule_User_Authenticate::getClass() const
{
  static const AString CLASS("User.Authenticate");
  return CLASS;
}

AOSModule_User_Authenticate::AOSModule_User_Authenticate(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_User_Authenticate::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AString strRetryPath;
  const AXmlElement *pRetryPage = moduleParams.findElement(AOS_User_Constants::PARAM_REDIRECT_RETRYPAGE);
  if (!pRetryPage)
  {
    //a_No retry page
    context.addError(getClass(), ARope("Missing module parameter for redirect on retry: ")+AOS_User_Constants::PARAM_REDIRECT_RETRYPAGE);
    return AOSContext::RETURN_ERROR;
  }
  else
  {
    if (!AOSContextUtils::getContentWithReference(context, *pRetryPage, strRetryPath))
    {
      context.useEventVisitor().startEvent(ARope("AOSModule_User_Authenticate: Missing retry redirect: ")+*pRetryPage, AEventVisitor::EL_ERROR);
      return AOSContext::RETURN_ERROR;
    }
  }

  AString strFailurePath;
  const AXmlElement *pFailPage = moduleParams.findElement(AOS_User_Constants::PARAM_REDIRECT_FAILURE);
  if (!pFailPage)
  {
    //a_No retry page
    context.addError(getClass(), ARope("Missing module parameter for redirect on fail: ")+AOS_User_Constants::PARAM_REDIRECT_FAILURE);
    return AOSContext::RETURN_ERROR;
  }
  else
  {
    if (!AOSContextUtils::getContentWithReference(context, *pFailPage, strFailurePath))
    {
      context.useEventVisitor().startEvent(ARope("AOSModule_User_Authenticate: Missing fail redirect: ")+*pFailPage, AEventVisitor::EL_ERROR);
      return AOSContext::RETURN_ERROR;
    }
  }

  //
  //TODO: This is where the check will go, for now just set the LoggedIn regardless of input
  //
  AString password;
  bool isValidUser = context.useRequestParameterPairs().get(AOS_User_Constants::PASSWORD, password);
  //isValidUser &= password.equals(ASWNL("60c6d277a8bd81de7fdde19201bf9c58a3df08f4"));
  if (isValidUser)
  {
    //a_Remove fail count on success
    context.useSessionData().useData().remove(AOS_User_Constants::SESSION_LOGINFAILCOUNT);

    //a_Add username, id and isLoggedIn to the user element in the SESSION
    AXmlElement& user = context.useSessionData().useData().overwriteElement(AOS_User_Constants::SESSION_USER);
    
    AString str;
    context.useRequestParameterPairs().get(AOS_User_Constants::USERNAME, str);
    user.overwriteElement(AOSUser::USERNAME).setData(str);
    user.overwriteElement(AOSUser::IS_LOGGED_IN);

    str.clear();
    if (context.useSessionData().useData().emitContentFromPath(AOS_User_Constants::SESSION_REDIRECTURL, str))
    {
      //a_Success redirect in session, if not there then just continue to execute
      context.setResponseRedirect(str);
      context.useSessionData().useData().remove(AOS_User_Constants::SESSION_REDIRECTURL);

      if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
        context.useEventVisitor().startEvent(AString(getClass())+": User authenticated, redirecting to: "+str, AEventVisitor::EL_DEBUG);

      return AOSContext::RETURN_REDIRECT;
    }
    else
    {
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
        context.useEventVisitor().startEvent(AString(getClass())+": User authenticated, no redirect found in session at: "+AOS_User_Constants::SESSION_REDIRECTURL, AEventVisitor::EL_DEBUG);

      return AOSContext::RETURN_OK;
    }
  }
  else
  {
    AString str;
    AString attempts;
    if (context.useSessionData().useData().emitContentFromPath(AOS_User_Constants::SESSION_LOGINFAILCOUNT, str))
    {
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
        context.useEventVisitor().startEvent(AString(getClass())+": User authention failed, count: "+str, AEventVisitor::EL_DEBUG);

      //a_Increment failure
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
      //a_First failure, add count
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
        context.useEventVisitor().startEvent(AString(getClass())+": User authention failed, count: 0", AEventVisitor::EL_DEBUG);

      context.useSessionData().useData().setInt(AOS_User_Constants::SESSION_LOGINFAILCOUNT, 1);
      context.setResponseRedirect(strFailurePath);
      return AOSContext::RETURN_REDIRECT;
    }
  }
  
  return AOSContext::RETURN_OK;
}
