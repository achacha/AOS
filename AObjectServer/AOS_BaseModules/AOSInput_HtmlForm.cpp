#include "pchAOS_BaseModules.hpp"
#include "AOSInput_HtmlForm.hpp"

const AString& AOSInput_HtmlForm::getClass() const
{
  static const AString CLASS("application/x-www-form-urlencoded");
  return CLASS;
}

AOSInput_HtmlForm::AOSInput_HtmlForm(AOSServices& services) :
  AOSInputProcessorInterface(services)
{
}

AOSContext::ReturnCode AOSInput_HtmlForm::execute(AOSContext& context)
{
  AString str;
  
  //a_Read FORM data if any
  if (context.useRequestHeader().getPairValue(AHTTPHeader::HT_ENT_Content_Length, str))
  {
    //a_Content specified
    u4 toRead = str.toU4();
    str.clear();
    context.useSocket().read(str, toRead);
    
    //a_Add POST variables to request query
    context.useRequestParameterPairs().parse(str);
    
    //a_Publish raw_data if dumping context in debug mode and when 'dumpContext' exists
    //a_The executor was supress all output if dumpContext is not enabled
    if (context.useRequestParameterPairs().exists(ASW("dumpContext",11)))
      context.useModel().addElement(ASW("input/data", 10)).addData(str, AXmlElement::ENC_CDATASAFE);
  }
  else
  {
    if (AHTTPRequestHeader::METHOD_ID_POST == context.useRequestHeader().getMethodId())
    {
      //a_411 Length Required if POST
      context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_411_Length_Required);
      return AOSContext::RETURN_ERROR;
    }
  }

  return AOSContext::RETURN_OK;
}
