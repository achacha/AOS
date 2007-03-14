#include "pchAOS_BaseModules.hpp"
#include "AOSModule_PublishInput.hpp"
#include "AQueryString.hpp"

const AString& AOSModule_PublishInput::getClass() const
{
  static const AString CLASS("PublishInput");
  return CLASS;
}

AOSModule_PublishInput::AOSModule_PublishInput(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_PublishInput::execute(AOSContext& context, const AXmlElement& params)
{
  context.useOutputRootXmlElement().addElement(ASW("/input/request_header", 21), context.useRequestHeader());
  context.useOutputRootXmlElement().addElement(ASW("/output/response_header", 23), context.useResponseHeader());

  return true;
}

