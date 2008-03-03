#include "pchAOS_BaseModules.hpp"
#include "AOSModule_SaveToFile.hpp"
#include "AXmlElement.hpp"
#include "AFile_Physical.hpp"

const AString& AOSModule_SaveToFile::getClass() const
{
  static const AString MODULE_CLASS("SaveToFile");
  return MODULE_CLASS;
}

AOSModule_SaveToFile::AOSModule_SaveToFile(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_SaveToFile::execute(AOSContext& context, const AXmlElement& params)
{
  AString objectReference;
  AString path;
  const AXmlElement *p = params.findElement(ASW("model-path",10));
  if (p)
  {
    p->getAttributes().get(ASW("reference",9), objectReference);
    p->emitContent(path);
  }
  else
  {
    context.addError(getClass(), ASWNL("Unable to find module/model-path"));
    return AOSContext::RETURN_ERROR;
  }

  AString filename;
  if (!params.emitContentFromPath(ASW("output-filename",15), filename))
  {
    context.addError(getClass(), ASWNL("Unable to find module/output-filename"));
    return AOSContext::RETURN_ERROR;
  }

  //TODO: build aos_data based path and save content
  AFilename f(m_Services.useConfiguration().getAosBaseDataDirectory(), filename, false);
  AFile_Physical outfile(f, "w");

  ARope rope;
  if (objectReference.equals(ASW("context-object",14)))
  {
    //a_Reference to context object
    const AEmittable *pObject = context.useContextObjects().getAsPtr<const AEmittable>(path);
    if (pObject)
      pObject->emit(rope);
    else
      context.addError(getClass(), ARope("Context object not found or not AEmittable-type at: ")+path);
  }
  else
  {
    //a_Save actual context
    context.useModel().emitContentFromPath(path, rope);
  }

  try
  {
    outfile.open();
    outfile.write(rope);
    outfile.close();
  }
  catch(AException& ex)
  {
    context.addError(getClass(), ARope("Unable to write file: ")+outfile.useFilename());
    return AOSContext::RETURN_ERROR;
  }

  return AOSContext::RETURN_OK;
}

