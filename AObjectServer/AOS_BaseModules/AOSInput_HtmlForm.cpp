/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSInput_HtmlForm.hpp"

const AString AOSInput_HtmlForm::CLASS("application/x-www-form-urlencoded");

const AString& AOSInput_HtmlForm::getClass() const
{
  return CLASS;
}

AOSInput_HtmlForm::AOSInput_HtmlForm(AOSServices& services) :
  AOSInputProcessorInterface(services)
{
}

AOSContext::ReturnCode AOSInput_HtmlForm::execute(AOSContext& context)
{
  if (AHTTPRequestHeader::METHOD_ID_POST == context.useRequestHeader().getMethodId())
  {
    //a_Read FORM data if any
    AString strLength;
    if (context.useRequestHeader().get(AHTTPHeader::HT_ENT_Content_Length, strLength))
    {
      //a_Content specified
      size_t toRead = strLength.toU4();
      AString str(toRead+128, 1024);
      size_t bytesRead = AConstant::unavail;
      while (AConstant::unavail == bytesRead)
      {
        bytesRead = context.useSocket().read(str, toRead);
        if (AConstant::npos == bytesRead)
        {
          context.addError(getClass(), ASWNL("AOSInput_HtmlForm: unable to read form data"));
          return AOSContext::RETURN_ERROR;
        }
        else if (AConstant::unavail)
        {
          AThread::sleep(3);
        }
      }

      if (bytesRead != toRead)
      {
        context.addError(getClass(), AString("Failed to read ")+AString::fromSize_t(toRead)+ASWNL(" bytes from socket, read only ")+AString::fromSize_t(bytesRead));
        return AOSContext::RETURN_ERROR;
      }

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
        context.useEventVisitor().startEvent(getClass()+ASW(": Content-Length missing, http error 411",40), AEventVisitor::EL_WARN);
        context.useResponseHeader().set(AHTTPResponseHeader::HT_GEN_Connection, ASW("close",5));
        context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_411_Length_Required);
        return AOSContext::RETURN_ERROR;
      }
    }
  }
  else
  {
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
    {
      context.useEventVisitor().startEvent(ARope("Non-POST request, skipping ",27)+getClass(), AEventVisitor::EL_DEBUG);
    }
  }

  return AOSContext::RETURN_OK;
}
