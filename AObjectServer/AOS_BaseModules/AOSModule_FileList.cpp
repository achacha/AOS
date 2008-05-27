/*
Written by Alex Chachanashvili

Id: $Id$
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
  static const AString FILE("file",4);

  AString str(1536, 1024);
  const AXmlElement *pePath = moduleParams.findElement(ASW("path", 4));
  if (!pePath)
  {
    context.useEventVisitor().addEvent(ASWNL("AOSModule_FileList: Unable to find 'path' parameter"), AEventVisitor::EL_ERROR);
    return AOSContext::RETURN_ERROR;
  }

  //a_Get the path content
  pePath->emitContent(str);

  AFilename *pFilename = NULL;
  AString strBase;
  if (pePath->getAttributes().get(ASW("base", 4), strBase))
  {
    if (strBase.equals(ASW("dynamic",7)))
    {
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDynamicDirectory());
      pFilename->join(str, false);
    }
    else if (strBase.equals(ASW("data",4)))
    {
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDataDirectory());
      pFilename->join(str, false);
    }
    else
    {
      //a_Default to static
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseStaticDirectory());
      pFilename->join(str, false);
    }
  }
  else
  {
    //a_Fallthru default case to static
    pFilename = new AFilename(m_Services.useConfiguration().getAosBaseStaticDirectory());
    pFilename->join(str, false);
  }

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
    offsetPath.usePathNames().pop_back();
    fileList.addElement(ASW("up",2)).addData(offsetPath);
  }
  else
  {
    //a_Base offset is .
    fileList.addElement(ASW("up",2)).addData(AConstant::ASTRING_EMPTY);
    fileList.addElement(ASW("offset",6)).addData(ASW("./",2));
  }
  
  //a_Build the file info
  AFileSystem::LIST_FileInfo files;
  if (AFileSystem::dir(*pFilename, files, false, false) > 0)
  {
    fileList.useAttributes().insert(ASW("count",5), AString::fromSize_t(files.size()));
    for(AFileSystem::LIST_FileInfo::iterator it = files.begin(); it != files.end(); ++it)
    {
      it->emitXml(fileList.addElement(FILE));
    }
  }
  else
    fileList.useAttributes().insert(ASW("count",5), ASW("0",1));

  return AOSContext::RETURN_OK;
}
