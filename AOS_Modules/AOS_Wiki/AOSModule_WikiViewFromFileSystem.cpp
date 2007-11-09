#include "pchAOS_Wiki.hpp"
#include "AOSModule_WikiViewFromFileSystem.hpp"
#include "AFilename.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ATextGenerator.hpp"

const AString& AOSModule_WikiViewFromFileSystem::getClass() const
{
  static const AString CLASS("WikiViewFromFileSystem");
  return CLASS;
}

AOSModule_WikiViewFromFileSystem::AOSModule_WikiViewFromFileSystem(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_WikiViewFromFileSystem::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Get base path
  AString basePath;
  if (!moduleParams.emitFromPath(ASW("base-path",9), basePath))
  {
    context.addError(ASWNL("AOSModule_WikiViewFromFileSystem::execute",36),ASWNL("Unable to find module/base-path parameter"));
    return false;
  }
  AFilename wikifile(m_Services.useConfiguration().getAosBaseDataDirectory());
  wikifile.join(basePath, true);

  //a_Get relative wiki path
  AString str;
  context.useRequestParameterPairs().get(ASW("wikipath",8), str);
  wikifile.join(str, false);

  AString strData;
  if (context.useRequestParameterPairs().get(ASW("wiki.newdata",12), strData))
  {
    //a_New data submitted
    AFilename newwikifile(wikifile);

    //a_Temporary filename to use during the swap
    AFilename tempwikifile(newwikifile);
    tempwikifile.setExtension(ASW("_temporary_rename_",19));

    //a_Generate temporary filename
    AFileSystem::generateTemporaryFilename(newwikifile);

    //a_Make sure directories exist
    AFileSystem::createDirectories(newwikifile);

    //a_Save new data to temp filename
    AFile_Physical file(newwikifile, "wb");
    file.open();
    file.write(strData);
    file.close();


    //a_Rename temp to current
    AFileSystem::rename(wikifile, tempwikifile);
    AFileSystem::rename(newwikifile, wikifile);
    AFileSystem::remove(tempwikifile);
  }
  else
  {
    if (!AFileSystem::exists(wikifile))
    {
      //a_Signal that the wiki file does not exist
      context.useOutputRootXmlElement().overwriteElement(ASW("wiki/DoesNotExits",17));
    }
    else
    {
      AFile_Physical file(wikifile);
      context.useOutputRootXmlElement().overwriteElement(ASW("wiki/row/data",13)).addData(file, AXmlElement::ENC_CDATADIRECT);
    }
  }

  return true;
}
