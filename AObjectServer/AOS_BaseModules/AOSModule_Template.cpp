#include "pchAOS_BaseModules.hpp"
#include "AOSModule_Template.hpp"
#include "AFile_AString.hpp"
#include "AFile_Physical.hpp"
#include "ABasePtrHolder.hpp"
#include "AXmlElement.hpp"

const AString& AOSModule_Template::getClass() const
{
  static const AString MODULE_CLASS("Template");
  return MODULE_CLASS;
}

AOSModule_Template::AOSModule_Template(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_Template::execute(AOSContext& context, const AXmlElement& params)
{
  const AXmlElement *pNode = params.findNode(ASW("script",6));
  AAutoPtr<AFile> pFile;
  if (pNode)
  {
    //a_Element contains script
    pFile.reset(new AFile_AString());
    
    //a_Use content as a file
    pNode->emitContent(*pFile);
  }
  else
  {
    pNode = params.findNode(ASW("filename",8));
    if (pNode)
    {
      AString relativePath;
      pNode->emitContent(relativePath);

      //a_File to be used (may need caching for it, but for now keep it dynamic)
      AFilename f(m_Services.useConfiguration().getAosBaseDataDirectory(), true);
      f.join(relativePath, false);
      
      pFile.reset(new AFile_Physical(f));
      pFile->open();
    }
    else
      return false;  //a_Did not find either /params/script or /params/file
  }
  
  //a_Objects
  ABasePtrHolder objects;
  objects.insert(ATemplate::OBJECTNAME_MODEL, &context.useOutputXmlDocument());  //a_Add ALibrary required model AXmlDocument
  objects.insert(AOSContext::OBJECTNAME, &context);                              //a_Add AOS required AOSContext

  //a_Process and save output
  ARope ropeOutput;
  AAutoPtr<ATemplate> pTemplate(m_Services.createTemplate());
  pTemplate->fromAFile(*pFile);
  pTemplate->process(objects, ropeOutput);
  
  //a_Add template to debug
  if (context.getDumpContextLevel() > 0)
  {
    AString str("/debug/");
    str.append(getClass());
    str.append("/template");
    AXmlElement& base = context.useOutputRootXmlElement().addElement(str);
    pTemplate->emitXml(base);
  }

  //a_Insert output into outpath (if any)
  pNode = params.findNode(ASW("outpath",7));
  if (pNode)
  {
    AString xmlpath;
    pNode->emitContent(xmlpath);
    if (!xmlpath.isEmpty())
    {
      //a_Add output as CDATA
      context.useOutputRootXmlElement().addElement(xmlpath).addData(ropeOutput, AXmlElement::ENC_CDATADIRECT);
    }
  }

  return true;
}

