#include "pchAOS_BaseModules.hpp"
#include "AOSModule_InsertIntoModel.hpp"
#include "AQueryString.hpp"

const AString& AOSModule_InsertIntoModel::getClass() const
{
  static const AString CLASS("InsertIntoModel");
  return CLASS;
}

AOSModule_InsertIntoModel::AOSModule_InsertIntoModel(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_InsertIntoModel::execute(AOSContext& context, const AXmlElement& params)
{
  const AXmlNode *pDataNode = params.findNode(ASW("/params/module/data",19));
  if (pDataNode)
  {
    AString strPath;
    params.emitFromPath(ASW("/params/module/path",19), strPath);
    context.useOutputRootXmlElement().addContent(pDataNode->clone(), strPath);
  }
  else
  {
    context.addError(ASWNL("AOSModule_InsertIntoModel::execute"), ASWNL("Required element /module/data not found."));
  }

  return true;
}

