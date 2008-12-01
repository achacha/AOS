/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Wiki.hpp"
#include "AOSModule_Wiki_ViewFromFileSystem.hpp"
#include "AOS_Wiki_Constants.hpp"
#include "AFilename.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ATextGenerator.hpp"

const AString AOSModule_Wiki_ViewFromFileSystem::CLASS("Wiki.ViewFromFileSystem");

const AString& AOSModule_Wiki_ViewFromFileSystem::getClass() const
{
  return CLASS;
}

AOSModule_Wiki_ViewFromFileSystem::AOSModule_Wiki_ViewFromFileSystem(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_Wiki_ViewFromFileSystem::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Get base path
  AString basePath;
  if (!moduleParams.emitString(AOS_Wiki_Constants::PARAM_BASE_PATH, basePath))
  {
    context.addError(getClass(), ASWNL("Unable to find module/base-path parameter"));
    return AOSContext::RETURN_ERROR;
  }
  AFilename wikifile(m_Services.useConfiguration().getAosBaseDataDirectory());
  wikifile.join(basePath, true);

  //a_Get relative wiki path
  AString str;
  context.useRequestParameterPairs().get(ASW("wikipath",8), str);
  wikifile.join(str, false);

  u4 type = AFileSystem::getType(wikifile);
  //a_Directory only will get default index.html
  if (type & AFileSystem::Directory)
  {
    //a_Directory name specified
    wikifile.useFilename().assign(ASW("index.html",10));
  }
  else
    wikifile.setExtension(ASW(".html",5));
  
  AString strData;
  if (context.useRequestParameterPairs().get(ASW("wiki.newdata",12), strData))
  {
    //a_Check if authentication passed
    if (
      moduleParams.exists(AOS_Wiki_Constants::PARAM_SECURE)
      && !context.useModel().exists(ASW("wiki/Authenticated",18))
    )
    {
      context.useModel().overwriteElement(ASW("wiki/AuthFailed",15));
      return AOSContext::RETURN_OK;
    }
    
    //a_New data submitted
    u4 type = AFileSystem::getType(wikifile);
    if (AFileSystem::DoesNotExist != type)
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
      context.useModel().overwriteElement(AOS_Wiki_Constants::ELEMENT_DATA).addData(strData, AXmlElement::ENC_CDATADIRECT);

      //a_Rename temp to current
      AFileSystem::rename(wikifile, tempwikifile);
      AFileSystem::rename(newwikifile, wikifile);
      AFileSystem::remove(tempwikifile);
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
      context.useModel().overwriteElement(AOS_Wiki_Constants::ELEMENT_DATA).addData(strData, AXmlElement::ENC_CDATADIRECT);
    }
  }
  else
  {
    u4 type = AFileSystem::getType(wikifile);
    if (AFileSystem::DoesNotExist != type)
    {
      AFile_Physical file(wikifile);
      file.open();
      context.useModel().overwriteElement(AOS_Wiki_Constants::ELEMENT_DATA).addData(file, AXmlElement::ENC_CDATADIRECT);
    }
    else
    {
      //a_Signal that the wiki file does not exist
      context.useModel().overwriteElement(AOS_Wiki_Constants::ELEMENT_DOES_NOT_EXIST);
    }

    if (moduleParams.exists(AOS_Wiki_Constants::PARAM_SECURE))
    {
      context.useModel().overwriteElement(AOS_Wiki_Constants::ELEMENT_SECURE_EDIT);
    }

  }

  return AOSContext::RETURN_OK;
}
