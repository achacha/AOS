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
  if (!moduleParams.emitString(ASW("path", 4), str))
  {
    context.useEventVisitor().addEvent(ASWNL("AOSModule_FileList: Unable to find 'path' parameter"), AEventVisitor::EL_ERROR);
    return AOSContext::RETURN_ERROR;
  }
  
  AFilename *pFilename = NULL;
  AString strBase;
  if (moduleParams.emitString(ASW("base", 4), strBase))
  {
    if (strBase.equals(ASW("data",4)))
    {
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDataDirectory());
      pFilename->join(str, false);
    }
    else if (strBase.equals(ASW("dynamic",7)))
    {
      pFilename = new AFilename(m_Services.useConfiguration().getAosBaseDynamicDirectory());
      pFilename->join(str, false);
    }
    else if (strBase.equals(ASW("absolute",8)))
    {
      pFilename = new AFilename(str, false);
    }
  }
  
  if (!pFilename)
  {
    pFilename = new AFilename(m_Services.useConfiguration().getAosBaseStaticDirectory());
    pFilename->join(str, false);
  }

  //a_Build the file info
  AXmlElement& fileList = context.useModel().addElement(ASW("file-list",9));
  AFileSystem::LIST_FileInfo files;
  if (AFileSystem::dir(*pFilename, files) > 0)
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

