/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_Redirect.hpp"
#include "AOS_BaseModules_Constants.hpp"

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
  //a_A command places the redirect into session or model or coded into the command itself
  //a_NOTE: redirect URL can never come from the query string for security reasons
  AString str(4096, 1024);
  const AXmlElement *pElement = context.getOutputParams().findElement(ASW("redirect",8));
  if (!pElement)
  {
    context.addError(getClass(), ASWNL("Missing element redirect"), true);
    return AOSContext::RETURN_ERROR;
  }
  if (!AOSContextUtils::getContentWithReference(context, *pElement, str))
  {
    context.addError(getClass(), ARope("Unable to find redirect path: ")+*pElement, true);
    return AOSContext::RETURN_ERROR;
  }

  //a_If redirect is empty, then do nothing
  if (!str.isEmpty())
  {
    //a_Check is forcing secure/non-secure protocol
    AUrl url(str);
    if (context.getOutputParams().exists(AOS_BaseModules_Constants::SECURE))
    {
      if (context.getOutputParams().getBool(AOS_BaseModules_Constants::SECURE, false))
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
