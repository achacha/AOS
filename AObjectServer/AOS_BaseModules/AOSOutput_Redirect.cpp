/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_Redirect.hpp"
#include "AOS_BaseModules_Constants.hpp"

const AString AOSOutput_Redirect::CLASS("Redirect");

const AString& AOSOutput_Redirect::getClass() const
{
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
  
  AXmlElement::CONST_CONTAINER redirects;
  if (0 == context.getOutputParams().find(ASW("redirect",8), redirects))
  {
    context.addError(getClass(), ASWNL("Missing element redirect"), true);
    return AOSContext::RETURN_ERROR;
  }
  
  bool redirectFound = false;
  for (AXmlElement::CONST_CONTAINER::iterator it = redirects.begin(); it != redirects.end(); ++it)
  {
    const AXmlElement *pElement = (*it);
    if (AOSContextUtils::getContentWithReference(context, *pElement, str))
    {
      redirectFound = true;
      break;
    }
  }

  if (!redirectFound)
  {
    context.addError(getClass(), ARope("Unable to find valid redirect: ")+context.getOutputParams(), true);
    return AOSContext::RETURN_ERROR;
  }

  //a_If redirect is empty, then do nothing
  if (!str.isEmpty())
  {
    //a_Check is forcing secure/non-secure protocol
    AUrl url(context.useRequestUrl());
    url.useParameterPairs().clear();        //a_Query string is not promoted in redirect
    url &= AUrl(str);
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
    
    str.clear();
    url.emit(str);
    context.setResponseRedirect(str);
    return AOSContext::RETURN_REDIRECT;
  }
  else
  {
    context.useEventVisitor().startEvent(ASW("Redirect location is empty, continuing",38));
    return AOSContext::RETURN_OK;
  }
}
