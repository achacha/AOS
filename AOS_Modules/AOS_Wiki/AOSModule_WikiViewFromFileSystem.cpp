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
  static const AString ELEMENT_IS_DIRECTORY("wiki/IsDirectory",16);
  static const AString ELEMENT_SECURE_EDIT("wiki/SecureEdit",15);
  static const AString ELEMENT_DOES_NOT_EXIST("wiki/DoesNotExist",17);
  static const AString ELEMENT_DATA("wiki/row/data",13);
  
  static const AString PARAM_SECURE("secure",6);
  static const AString PARAM_BASE_PATH("base-path",9);

  //a_Get base path
  AString basePath;
  if (!moduleParams.emitFromPath(PARAM_BASE_PATH, basePath))
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
  if (
    context.useRequestParameterPairs().get(ASW("wiki.newdata",12), strData)
  )
  {
    //a_Check if authentication passed
    if (
      moduleParams.exists(PARAM_SECURE)
      && !context.useOutputRootXmlElement().exists(ASW("wiki/Authenticated",18))
    )
    {
      context.useOutputRootXmlElement().overwriteElement(ASW("wiki/AuthFailed",15));
      return true;
    }
    
    //a_New data submitted
    u4 type = AFileSystem::getType(wikifile);
    if (type)
    {
      if (type & AFileSystem::Directory)
      {
        //a_Directory name specified
        context.useOutputRootXmlElement().overwriteElement(ELEMENT_IS_DIRECTORY);
      }
      else
      {
        //a_New data submitted, old file exists
        AFilename newwikifile(wikifile);

        //a_Temporary filename to use during the swap
        AFilename tempwikifile(newwikifile);
        tempwikifile.setExtension(ASW("_temporary_rename_",19));

        //a_Generate temporary filename
        AFileSystem::generateTemporaryFilename(newwikifile);

        //a_Save new data to temp filename
        AFile_Physical file(newwikifile, "wb");
        file.open();
        file.write(strData);
        file.close();

        //a_Add to context so it can ve viewed after save
        context.useOutputRootXmlElement().overwriteElement(ELEMENT_DATA).addData(strData, AXmlElement::ENC_CDATADIRECT);

        //a_Rename temp to current
        AFileSystem::rename(wikifile, tempwikifile);
        AFileSystem::rename(newwikifile, wikifile);
        AFileSystem::remove(tempwikifile);
      }
    }
    else
    {
      //a_Does not exist yet
      //a_Make sure directories exist
      AFileSystem::createDirectories(wikifile);

      //a_Save new data
      AFile_Physical file(wikifile, "wb");
      file.open();
      file.write(strData);
      file.close();

      //a_Add to context so it can ve viewed after save
      context.useOutputRootXmlElement().overwriteElement(ELEMENT_DATA).addData(strData, AXmlElement::ENC_CDATADIRECT);
    }
  }
  else
  {
    u4 type = AFileSystem::getType(wikifile);
    if (type)
    {
      if (type & AFileSystem::Directory)
      {
        //a_Directory name specified
        context.useOutputRootXmlElement().overwriteElement(ELEMENT_IS_DIRECTORY);
      }
      else
      {
        AFile_Physical file(wikifile);
        file.open();
        context.useOutputRootXmlElement().overwriteElement(ELEMENT_DATA).addData(file, AXmlElement::ENC_CDATADIRECT);
      }
    }
    else
    {
      //a_Signal that the wiki file does not exist
      if (wikifile.isDirectory())
        context.useOutputRootXmlElement().overwriteElement(ELEMENT_IS_DIRECTORY);
      else
        context.useOutputRootXmlElement().overwriteElement(ELEMENT_DOES_NOT_EXIST);
    }

    if (moduleParams.exists(PARAM_SECURE))
    {
      context.useOutputRootXmlElement().overwriteElement(ELEMENT_SECURE_EDIT);
    }

  }

  return true;
}
