/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_PublishInput.hpp"

const AString& AOSModule_PublishInput::getClass() const
{
  static const AString CLASS("PublishInput");
  return CLASS;
}

AOSModule_PublishInput::AOSModule_PublishInput(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_PublishInput::execute(AOSContext& context, const AXmlElement& params)
{
  context.useModel().overwriteElement(ASW("input/request_header", 20)).addData(context.useRequestHeader(), AXmlElement::ENC_CDATADIRECT);
  context.useModel().overwriteElement(ASW("output/response_header", 22)).addData(context.useResponseHeader(), AXmlElement::ENC_CDATADIRECT);

  return AOSContext::RETURN_OK;
}

