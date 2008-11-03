/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_FileList.hpp"
#include "AFilename.hpp"
#include "AFileSystem.hpp"

const AString& AOSModule_FileList::getClass() const
{
  static const AString MODULE_CLASS("FileList");
  return MODULE_CLASS;
}

AOSModule_FileList::AOSModule_FileList(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_FileList::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AString str(1536, 1024);
  const AXmlElement *pePath = moduleParams.findElement(AOS_BaseModules_Constants::FILENAME);
  if (!pePath)
  {
    context.useEventVisitor().addEvent(ASWNL("AOSModule_FileList: Unable to find 'path' parameter"), AEventVisitor::EL_ERROR);
    return AOSContext::RETURN_ERROR;
  }

  //a_Get the path content
  pePath->emitContent(str);

  AAutoPtr<AFilename> pFilename;
  AString strBase;
  if (pePath->getAttributes().get(AOS_BaseModules_Constants::BASE, strBase))
  {
    if (strBase.equals(ASW("dynamic",7)))
    {
      pFilename.reset(new AFilename(m_Services.useConfiguration().getAosBaseDynamicDirectory()));
      pFilename->join(str, false);
    }
    else if (strBase.equals(ASW("data",4)))
    {
      pFilename.reset(new AFilename());
      m_Services.useConfiguration().getAosDataDirectory(context, *pFilename);
      pFilename->join(str, false);
    }
    else if (strBase.equals(ASW("absolute",8)))
    {
      pFilename.reset(new AFilename(str, false));
    }
    else
    {
      //a_Default to static
      pFilename.reset(new AFilename());
      m_Services.useConfiguration().getAosStaticDirectory(context, *pFilename);
      pFilename->join(str, false);
    }
  }
  else
  {
    //a_Fallthru default case to static
    pFilename.reset(new AFilename());
    m_Services.useConfiguration().getAosStaticDirectory(context, *pFilename);
    pFilename->join(str, false);
  }

  //a_Properties
  bool recursive = moduleParams.getBool(ASW("recursive",9), false);

  //a_File list base element
  AXmlElement& fileList = context.useModel().addElement(ASW("file-list",9));

  //a_Offset to the current base
  AString strOffset;
  if (context.useRequestParameterPairs().get(ASW("offset",6), strOffset))
  {
    AFilename offsetPath(strOffset, true);
    if (!offsetPath.compactPath())
    {
      //The relative path goes below base
      context.useEventVisitor().addEvent(ASWNL("AOSModule_FileList: relative path has negative redirect beyond base"), AEventVisitor::EL_ERROR);
      return AOSContext::RETURN_ERROR;
    }
    pFilename->join(offsetPath, false);

    //a_Add offset and one up
    fileList.addElement(ASW("offset",6)).addData(offsetPath);
    if(offsetPath.usePathNames().size() > 0)
    {
      offsetPath.usePathNames().pop_back();
      fileList.addElement(ASW("up",2)).addData(offsetPath);
    }
  }
  else
  {
    //a_Base offset is .
    fileList.addElement(ASW("up",2)).addData(AConstant::ASTRING_EMPTY);
    fileList.addElement(ASW("offset",6)).addData(ASW("./",2));
  }
  
  //a_Build the file info
  AFileSystem::FileInfos files;
  if (AFileSystem::dir(*pFilename, files, false, !recursive) > 0)
  {
    files.sort();
    fileList.useAttributes().insert(ASW("count",5), AString::fromSize_t(files.size()));
    for(AFileSystem::FileInfos::iterator it = files.begin(); it != files.end(); ++it)
    {
      if (it->filename.compactPath())
        it->emitXml(fileList.addElement(AOS_BaseModules_Constants::S_FILE));
    }
  }
  else
    fileList.useAttributes().insert(ASW("count",5), ASW("0",1));

  return AOSContext::RETURN_OK;
}
