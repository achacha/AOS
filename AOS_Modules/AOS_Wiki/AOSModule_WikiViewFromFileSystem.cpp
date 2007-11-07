#include "pchAOS_Wiki.hpp"
#include "AOSModule_WikiViewFromFileSystem.hpp"
#include "AFilename.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"

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

  return true;
}
