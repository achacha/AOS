#include "pchAOS_User.hpp"
#include "AOSModule_LoginController.hpp"

const AString& AOSModule_LoginController::getClass() const
{
  static const AString CLASS("User.LoginController");
  return CLASS;
}

AOSModule_LoginController::AOSModule_LoginController(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_LoginController::execute(AOSContext& context, const AXmlElement& moduleParams)
{  
  //a_Save redirect URL based on reported values
  AUrl url(context.useRequestUrl());
  m_Services.useConfiguration().convertUrlToReportedServerAndPort(url);
  context.useSessionData().useData()
    .overwriteElement(AOS_User_Constants::SESSION_REDIRECTURL)
      .addData(url, AXmlData::ENC_CDATADIRECT);

  //a_Check if user is logged in
  if (!context.useSessionData().useData().exists(AOS_User_Constants::SESSION_ISLOGGEDIN))
  {
    AString str;
    if (moduleParams.emitString(AOS_User_Constants::PARAM_REDIRECT_LOGINPAGE, str))
    {
      //a_Redirect to login page
      context.useEventVisitor().startEvent(ASW("User not logged in, redirecting",31));
      if (moduleParams.exists(AOS_User_Constants::PARAM_SECURE))
      {
        AUrl url(str);
        url.setProtocol(AUrl::HTTPS);
        context.setResponseRedirect(url);
      }
      else
        context.setResponseRedirect(str);
      
      return AOSContext::RETURN_REDIRECT;
    }
    else
    {
      //a_Missing parameter for redirect
      context.addError(getClass(), ASWNL("Missing required module parameter 'redirect'"));
      return AOSContext::RETURN_ERROR;
    }
  }
  
  return AOSContext::RETURN_OK;
}
