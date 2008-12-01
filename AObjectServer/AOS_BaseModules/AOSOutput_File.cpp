/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_File.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"

const AString AOSOutput_File::CLASS("File");

const AString& AOSOutput_File::getClass() const
{
  return CLASS;
}

AOSOutput_File::AOSOutput_File(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_File::execute(AOSContext& context)
{
  const AXmlElement *peFilename = context.getOutputParams().findElement(AOS_BaseModules_Constants::FILENAME);
  if (!peFilename)
  {
    context.useEventVisitor().addEvent(ASWNL("AOSOutput_File: Unable to find 'filename' parameter"), AEventVisitor::EL_ERROR);
    return AOSContext::RETURN_ERROR;
  }

  //a_Get filename requested
  AString str(1536, 1024);
  peFilename->emitContent(str);

  AAutoPtr<AFilename> pFilename;
  AString strBase;
  if (peFilename->getAttributes().get(AOS_BaseModules_Constants::BASE, strBase))
  {
    if (strBase.equals(ASW("data",4)))
    {
      pFilename.reset(new AFilename());
      m_Services.useConfiguration().getAosDataDirectory(context, *pFilename);
      pFilename->join(str, false);
    }
    else if (strBase.equals(ASW("dynamic",7)))
      pFilename.reset(new AFilename(m_Services.useConfiguration().getAosBaseDynamicDirectory(), str, false));
    else if (strBase.equals(ASW("absolute",8)))
      pFilename.reset(new AFilename(str, false));
  }

  //a_Static is the default
  if (!pFilename)
  {
    pFilename.reset(new AFilename());
    m_Services.useConfiguration().getAosStaticDirectory(context, *pFilename);
    pFilename->join(str, false);
  }

  if (!AFileSystem::exists(*pFilename))
  {
    context.addError(ASWNL("AOSOutput_File"), ARope("File not found: ",16)+*pFilename);
    return AOSContext::RETURN_ERROR;
  }

  //a_See if extension for mime type set
  AString ext;
  if (context.getOutputParams().emitContentFromPath(AOS_BaseModules_Constants::MIME_EXTENSION, ext))
  {
    m_Services.useConfiguration().setMimeTypeFromExt(ext, context);
  }
  else
  {
    //a_Set content type based on file extension
    AString ext;
    pFilename->emitExtension(ext);
    m_Services.useConfiguration().setMimeTypeFromExt(ext, context);
  }

  context.clearOutputBuffer();
  AFile_Physical file(*pFilename);
  file.open();
  file.readUntilEOF(context.useOutputBuffer());
  file.close();

  return AOSContext::RETURN_OK;
}
