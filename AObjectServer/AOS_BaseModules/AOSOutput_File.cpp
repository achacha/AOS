#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_File.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"

const AString& AOSOutput_File::getClass() const
{
  static const AString CLASS("File");
  return CLASS;
}

AOSOutput_File::AOSOutput_File(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_File::execute(AOSContext& context)
{
 
  AString str(1536, 1024);
  if (!context.getOutputParams().emitString(ASW("path", 4), str))
  {
    context.useEventVisitor().set(ASWNL("AOSOutput_File: Unable to find 'path' parameter"), true);
    return AOSContext::RETURN_ERROR;
  }
  
  AFilename *pFilename = NULL;
  AString strBase;
  if (context.getOutputParams().emitString(ASW("base", 4), strBase))
  {
    if (strBase.equals(ASW("static",6)))
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseStaticDirectory(), str, false);
    else if (strBase.equals(ASW("dynamic",7)))
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDynamicDirectory(), str, false);
    else if (strBase.equals(ASW("absolute",8)))
      pFilename = new AFilename(str, false);
  }

  //a_Data is the default
  if (!pFilename)
    pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDataDirectory(), str, false);
  
  if (!AFileSystem::exists(*pFilename))
  {
    context.addError(ASWNL("AOSOutput_File"), ARope("File not found: ",16)+*pFilename);
    return AOSContext::RETURN_ERROR;
  }

  //a_Optional content type, if none, use text/html
  AString contentType;
  const AXmlElement *pType = context.getOutputParams().findElement(ASW("content-type", 12));
  if (pType)
    pType->emitContent(contentType);
  else
    contentType.assign("text/html",9);

  //a_Set content type
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, contentType);

  if (
       context.useEventVisitor().getErrorCount() > 0
    || context.useRequestParameterPairs().exists(ASW("dumpContext", 11))
    )
  {
    //a_Put file data into output buffer, error in event visitor detected or dumpContext requested
    str.assign("AOSOutput_File: Outputting as (",31);
    str.append(contentType);
    str.append("): ",3);
    pFilename->emit(str);
    context.useEventVisitor().set(str);

    AFile_Physical file(*pFilename);
    file.open();
    file.readUntilEOF(context.useOutputBuffer());
    file.close();

    context.useEventVisitor().reset();
  }
  else
  {
    //a_Send file directly, no errors detected
    str.assign("AOSOutput_File: Sending as (",28);
    str.append(contentType);
    str.append("): ",3);
    pFilename->emit(str);
    context.useEventVisitor().set(str);

    //a_Send HTTP header and file content
    AFile_Physical file(*pFilename);
    file.open();
    context.useResponseHeader().emit(context.useSocket());
    context.useSocket().write(file);

    context.setOutputCommitted(true);
    context.useEventVisitor().reset();
  }
  
  return AOSContext::RETURN_OK;
}
