#include "pchAOS_SAMPLEPROJECT.hpp"
#include "AOSModule_SAMPLEMODULE.hpp"

const AString AOSModule_SAMPLEMODULE::CLASS("SAMPLEMODULE");

const AString& AOSModule_SAMPLEMODULE::getClass() const
{
  return CLASS;
}

AOSModule_SAMPLEMODULE::AOSModule_SAMPLEMODULE(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_SAMPLEMODULE::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  return AOSContext::RETURN_OK;
}
