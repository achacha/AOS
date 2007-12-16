#include "pchAOS_BaseModules.hpp"
#include "AOSModule_PublishInput.hpp"
#include "AQueryString.hpp"

const AString& AOSModule_PublishInput::getClass() const
{
  static const AString CLASS("PublishInput");
  return CLASS;
}

AOSModule_PublishInput::AOSModule_PublishInput(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_PublishInput::execute(AOSContext& context, const AXmlElement& params)
{
  context.useModel().addElement(ASW("input/request_header", 20)).addData(context.useRequestHeader(), AXmlElement::ENC_CDATADIRECT);
  context.useModel().addElement(ASW("output/response_header", 22)).addData(context.useResponseHeader(), AXmlElement::ENC_CDATADIRECT);

  return true;
}

