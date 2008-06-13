/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_User.hpp"
#include "AOSModule_User_LoginController.hpp"
#include "AOSUser.hpp"

const AString& AOSModule_User_LoginController::getClass() const
{
  static const AString CLASS("User.LoginController");
  return CLASS;
}

AOSModule_User_LoginController::AOSModule_User_LoginController(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_User_LoginController::execute(AOSContext& context, const AXmlElement& moduleParams)
{  
  //a_Check if user is logged in
  AXmlElement *pUser = context.useSessionData().useData().findElement(AOS_User_Constants::SESSION_USER);
  if (!pUser || !pUser->exists(AOSUser::IS_LOGGED_IN))
  {
    //a_Save redirect URL based on reported values
    AUrl url(context.useRequestUrl());
    m_Services.useConfiguration().convertUrlToReportedServerAndPort(url);
    context.useSessionData().useData()
      .overwriteElement(AOS_User_Constants::SESSION_REDIRECTURL)
        .setData(url, AXmlData::ENC_CDATADIRECT);
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
      context.useEventVisitor().startEvent(AString(getClass())+": Pushing redirect URL: "+url, AEventVisitor::EL_DEBUG);

    AString str;
    if (moduleParams.emitString(AOS_User_Constants::PARAM_REDIRECT_LOGINPAGE, str))
    {
      url.setPathAndFilename(str);
      
      //a_Redirect to login page
      context.useEventVisitor().startEvent(ASW("User not logged in, redirecting",31));
      if (moduleParams.exists(AOS_User_Constants::PARAM_SECURE))
      {
        url.setProtocol(AUrl::HTTPS);
        context.setResponseRedirect(url);
      }
      else
        context.setResponseRedirect(url);
      
      return AOSContext::RETURN_REDIRECT;
    }
    else
    {
      //a_Missing parameter for redirect
      context.addError(getClass(), AString("Missing required module parameter:")+AOS_User_Constants::PARAM_REDIRECT_LOGINPAGE);
      return AOSContext::RETURN_ERROR;
    }
  }

  //a_Force everything with this directory controller to not cache content
  context.useContextFlags().setBit(AOSContext::CTXFLAG_IS_CACHE_CONTROL_NO_CACHE);

  return AOSContext::RETURN_OK;
}
