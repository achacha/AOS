#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_File.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"

const AString& AOSOutput_File::getClass() const
{
  static const AString CLASS("File");
  return CLASS;
}

AOSOutput_File::AOSOutput_File(ALog& alog) :
  AOSOutputGeneratorInterface(alog)
{
}

bool AOSOutput_File::execute(AOSOutputContext& context)
{
 
  AString str(1536, 1024);
  if (!context.getOutputParams().emitFromPath(ASW("/params/output/path", 19), str))
  {
    context.useEventVisitor().set(ASWNL("AOSOutput_File: Unable to find 'path' parameter"), true);
    ATHROW_EX(this, AException::InvalidParameter, ASWNL("StaticFile requires 'path' parameter"));
  }
  
  AFilename *pFilename = NULL;
  AString strBase;
  if (context.getOutputParams().emitFromPath(ASW("/params/base", 12), strBase))
  {
    if (strBase.equals(ASW("static",6)))
      pFilename = new AFilename(context.getConfiguration().getAosBaseStaticDirectory(), str, false);
    else if (strBase.equals(ASW("dynamic",7)))
      pFilename = new AFilename(context.getConfiguration().getAosBaseDynamicDirectory(), str, false);
    else if (strBase.equals(ASW("absolute",8)))
      pFilename = new AFilename(str, false);
  }

  //a_Data is the default
  if (!pFilename)
    pFilename = new AFilename(context.getConfiguration().getAosBaseDataDirectory(), str, false);
  
  if (!AFileSystem::exists(*pFilename))
  {
    context.addError(ASWNL("AOSOutput_File"), ARope("File not found: ",16)+*pFilename);
    return true;
  }

  //a_Optional content type, if none, use text/html
  AString contentType;
  const AXmlNode *pType = context.getOutputParams().findNode(ASW("/params/content-type", 20));
  if (pType)
    pType->emitContent(contentType);
  else
    contentType.assign("text/html",9);

  //a_Set content type
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, contentType);

  if (
       context.useEventVisitor().getErrorCount() > 0
    || context.getRequestParameterPairs().exists(ASW("dumpContext", 11))
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
  return true;
}
