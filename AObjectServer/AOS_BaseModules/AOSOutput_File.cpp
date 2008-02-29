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
    context.useEventVisitor().addEvent(ASWNL("AOSOutput_File: Unable to find 'path' parameter"), AEventVisitor::EL_ERROR);
    return AOSContext::RETURN_ERROR;
  }
  
  AFilename *pFilename = NULL;
  AString strBase;
  if (context.getOutputParams().emitString(ASW("base", 4), strBase))
  {
    if (strBase.equals(ASW("data",4)))
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDataDirectory(), str, false);
    else if (strBase.equals(ASW("dynamic",7)))
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDynamicDirectory(), str, false);
    else if (strBase.equals(ASW("absolute",8)))
      pFilename = new AFilename(str, false);
  }

  //a_Static is the default
  if (!pFilename)
    pFilename = new AFilename(m_Services.useConfiguration().getAosBaseStaticDirectory(), str, false);
  
  if (!AFileSystem::exists(*pFilename))
  {
    context.addError(ASWNL("AOSOutput_File"), ARope("File not found: ",16)+*pFilename);
    return AOSContext::RETURN_ERROR;
  }

  //a_Optional content type, if none, use text/html
  const AXmlElement *pType = context.getOutputParams().findElement(ASW("content-type", 12));
  if (pType)
  {
    //a_Content-Type override in command
    AString contentType;
    pType->emitContent(contentType);
    context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, contentType);
  }
  else
  {
    //a_Set content type based on file extension
    AString ext;
    pFilename->emitExtension(ext);
    context.setResponseMimeTypeFromExtension(ext);
  }

  context.clearOutputBuffer();
  AFile_Physical file(*pFilename);
  file.open();
  file.readUntilEOF(context.useOutputBuffer());
  file.close();

  return AOSContext::RETURN_OK;
}
