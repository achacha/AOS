/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_Redirect.hpp"

const AString& AOSOutput_Redirect::getClass() const
{
  static const AString CLASS("Redirect");
  return CLASS;
}

AOSOutput_Redirect::AOSOutput_Redirect(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_Redirect::execute(AOSContext& context)
{
  //a_NOTE: redirect URL can never come from the query string for security reasons
  //a_A command places the redirect into session or model or coded into the command itself
  static const AString REDIRECT_URL("url",3);
  static const AString REDIRECT_PATH("model-path",10);
  static const AString SESSION_PATH("session-path",12);
  static const AString SECURE("secure",6);
  
  AString str(4096, 1024);
  AString path(1024, 256);
  if (context.getOutputParams().emitString(REDIRECT_URL, str))
  {
    //a_Url in command   
    context.useEventVisitor().startEvent(ARope("Redirect URL: ",14)+str);
    context.setResponseRedirect(str);
  }
  else if (context.getOutputParams().emitString(REDIRECT_PATH, path))
  {
    //a_Model
    if (context.useModel().emitString(path, str))
    {
      //a_Redirect to URL
      context.useEventVisitor().startEvent(ARope("Model Redirect[",15)+path+ASW("]: ",3)+str);
    }
    else
    {
      context.addError(getClass(), ARope("Redirect missing url in the model: ")+path, true);
      return AOSContext::RETURN_ERROR;
    }
  }
  else if (context.getOutputParams().emitString(SESSION_PATH, path))
  {
    //a_Session
    if (context.useSessionData().useData().emitString(path, str))
    {
      //a_Redirect to URL
      context.useEventVisitor().startEvent(ARope("Session Redirect[",17)+path+ASW("]: ",3)+str);
    }
    else
    {
      context.addError(getClass(), ARope("Redirect missing url in the session: ")+path, true);
      return AOSContext::RETURN_ERROR;
    }
  }
  else
  {
    context.addError(getClass(), ASWNL("Redirect missing parameters"), true);
    return AOSContext::RETURN_ERROR;
  }

  //a_If redirect is empty, then do nothing
  if (!str.isEmpty())
  {
    //a_Check is forcing secure/non-secure protocol
    AUrl url(str);
    if (context.getOutputParams().exists(SECURE))
    {
      if (context.getOutputParams().getBool(SECURE, false))
      {
        //a_HTTPS
        url.setProtocol(AUrl::HTTPS);
      }
      else
      {
        //a_HTTP
        url.setProtocol(AUrl::HTTP);
      }
    }
    
    context.setResponseRedirect(str);
    return AOSContext::RETURN_REDIRECT;
  }
  else
  {
    context.useEventVisitor().startEvent(ASW("Redirect location is empty, continuing",38));
    return AOSContext::RETURN_OK;
  }
}
