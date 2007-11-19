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
  const AXmlElement *pDataNode = params.findElement(ASW("data",4));
  if (pDataNode)
  {
    AString strPath;
    params.emitFromPath(ASW("path",4), strPath);
    context.useModel().addContent(pDataNode->clone(), strPath);
  }
  else
  {
    context.addError(ASWNL("AOSModule_InsertIntoModel::execute"), ASWNL("Required element /module/data not found."));
  }

  return true;
}

