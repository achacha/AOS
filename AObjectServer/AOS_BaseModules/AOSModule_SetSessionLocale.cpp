/*
Written by Alex Chachanashvili

$Id: AOSModule_SetSessionLocale.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_SetSessionLocale.hpp"

const AString& AOSModule_SetSessionLocale::getClass() const
{
  static const AString CLASS("SetSessionLocale");
  return CLASS;
}

AOSModule_SetSessionLocale::AOSModule_SetSessionLocale(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_SetSessionLocale::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Adding parameters
  const AXmlElement *p = moduleParams.findElement(AOSSessionData::LOCALE);
  if (p)
  {
    AString locale;
    p->emitContent(locale);
    
    if (locale.isEmpty())
    {
      //a_Remove locale from session and HTTP request, implies default
      {
        //a_Lock session and modify
        ALock lock(context.useSessionData().useSyncObject());
        context.useSessionData().useData().remove(AOSSessionData::LOCALE);
      }
      context.setLocaleOnRequestHeader(locale);
    }
    else
    {
      {
        //a_Lock session and modify
        ALock lock(context.useSessionData().useSyncObject());
        context.useSessionData().useData().overwriteElement(AOSSessionData::LOCALE).setData(locale);
      }
      context.setLocaleOnRequestHeader(locale);
    }
  }
  else
  {
    context.addError(getClass(), AString("Missing required module parameter: ")+AOSSessionData::LOCALE);
    return AOSContext::RETURN_ERROR;
  }

  return AOSContext::RETURN_OK;
}
