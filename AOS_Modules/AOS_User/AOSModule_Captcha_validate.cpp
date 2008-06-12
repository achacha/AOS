/*
Written by Alex Chachanashvili

$Id: AOSModule_rss20.cpp 89 2008-05-29 23:25:42Z achacha $
*/
#include "pchAOS_User.hpp"
#include "AOSModule_Captcha_validate.hpp"

const AString& AOSModule_Captcha_validate::getClass() const
{
  static const AString CLASS("Captcha.validate");
  return CLASS;
}

AOSModule_Captcha_validate::AOSModule_Captcha_validate(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_Captcha_validate::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  bool valid = false;

  AString captcha;
  if (context.useRequestParameterPairs().get(AOS_User_Constants::CAPTCHA, captcha))
  {
    //a_Compare to the session variable
    AString sessionOne;
    if (context.useSessionData().useData().emitContentFromPath(AOS_User_Constants::SESSION_CAPTCHA, sessionOne))
    {
      //a_Check if valid and remove from session
      valid = captcha.equals(sessionOne);
      context.useSessionData().useData().remove(AOS_User_Constants::SESSION_CAPTCHA);
    }
  }

  if (valid)
    context.useModel().overwriteElement(AOS_User_Constants::MODEL_CAPTCHA_VALID);
  else
    context.useModel().overwriteElement(AOS_User_Constants::MODEL_CAPTCHA_NOTVALID);
  
  return AOSContext::RETURN_OK;
}

