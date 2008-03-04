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
  ARope rope;
  if (objectReference.equals(ASW("context-object",14)))
  {       
    //a_Reference to context object
    AString variable;
    context.useModel().emitContentFromPath(path, variable);
    const AEmittable *pObject = context.useContextObjects().getAsPtr<const AEmittable>(variable);
    if (pObject)
      pObject->emit(rope);
    else
      context.addError(getClass(), ARope("Context object named '")+variable+ASWNL("' not found or not AEmittable-type at path: ")+path);

    //a_Set filename to the one from upload
    filename.clear();
    if (context.useModel().emitContentFromPath(path+".filename", filename))
      f.useFilename().assign(filename);
  }
  else
  {
    //a_Save actual context
    context.useModel().emitContentFromPath(path, rope);
  }

  if (AFileSystem::exists(f))
  {
    context.useModel().overwriteElement(AOSContext::S_ERROR).addData(ARope("File by that name already exists: ")+f);
    return AOSContext::RETURN_OK;
  }
  else
  {
    AFile_Physical outfile(f, "w");
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
  }

  return AOSContext::RETURN_OK;
}

