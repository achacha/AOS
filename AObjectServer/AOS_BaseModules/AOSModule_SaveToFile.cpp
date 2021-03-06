/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_SaveToFile.hpp"
#include "AXmlElement.hpp"
#include "AFile_Physical.hpp"

const AString AOSModule_SaveToFile::CLASS("SaveToFile");

const AString& AOSModule_SaveToFile::getClass() const
{
  return CLASS;
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
  const AXmlElement *pOutputFilename = params.findElement(ASW("output-filename",15));
  if (!pOutputFilename)
  {
    context.addError(getClass(), ASWNL("Unable to find module/output-filename"));
    return AOSContext::RETURN_ERROR;
  }
  else
  {
    pOutputFilename->emitContent(filename);
  }

  //a_data based path
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

    //a_See if output-filename is overiding the uploaded filename
    if (f.useFilename().isEmpty())
    {
      //a_Set filename to the one from upload if output-filename was empty
      filename.clear();
      if (context.useModel().emitContentFromPath(path+".filename", filename))
        f.useFilename().assign(filename);
      else
      {
        //a_This would only happen if the file is uploaded incorrectly and controller specifies a directory only
        context.addError(getClass(), ASWNL("Must have filename provided by upload if the controller is only specifying a directory"));
        return AOSContext::RETURN_ERROR;
      }
    }
  }
  else
  {
    //a_Save actual context
    context.useModel().emitContentFromPath(path, rope);
  }

  if (AFileSystem::exists(f) && !pOutputFilename->getAttributes().getBool(ASW("overwrite",9)))
  {
    context.useModel().overwriteElement(AOSContext::S_ERROR).addData(ARope("File by that name already exists: ")+f);
    return AOSContext::RETURN_OK;
  }

  AFile_Physical outfile(f, "wb");
  try
  {
    outfile.open();
    outfile.write(rope);
    outfile.close();
  }
  catch(AException& ex)
  {
    context.addError(getClass(), ARope("Unable to write file(")+outfile.useFilename()+ASW("):",2)+ex);
    return AOSContext::RETURN_ERROR;
  }

  return AOSContext::RETURN_OK;
}

