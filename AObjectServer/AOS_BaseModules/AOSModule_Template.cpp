/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_Template.hpp"
#include "AFile_Physical.hpp"

const AString& AOSModule_Template::getClass() const
{
  static const AString MODULE_CLASS("Template");
  return MODULE_CLASS;
}

AOSModule_Template::AOSModule_Template(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_Template::execute(AOSContext& context, const AXmlElement& params)
{
  const AXmlElement *pNode = params.findElement(ASW("template",8));
  AAutoPtr<AFile> pFile(NULL, false);
  if (pNode)
  {
    //a_Element contains script
    pFile.reset(new AFile_AString());
    
    //a_Use content as a file
    pNode->emitContent(*pFile);
  }
  else
  {
    pNode = params.findElement(ASW("filename",8));
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
    {
      context.addError(getClass(), ASWNL("Unable to find module/template nor module/filename, Template module did not execute, params"));
      return AOSContext::RETURN_ERROR;  //a_Did not find either module/template or module/filename
    }
  }  

  //a_Template
  AAutoPtr<ATemplate> pTemplate(m_Services.createTemplate(), true);
  pTemplate->fromAFile(*pFile);

  //a_Process and save output
  ARope ropeOutput;
  pTemplate->process(context.useLuaTemplateContext(), ropeOutput);
  
  //a_Add template to debug
  if (context.getDumpContextLevel() > 0)
  {
    AString str("debug/",6);
    str.append(getClass());
    str.append("/template",9);
    AXmlElement& base = context.useModel().addElement(str);
    pTemplate->emitXml(base);
  }

  //a_Insert output into outpath (if any)
  pNode = params.findElement(ASW("outpath",7));
  if (pNode)
  {
    AString xmlpath;
    pNode->emitContent(xmlpath);
    if (!xmlpath.isEmpty())
    {
      //a_Add output as CDATA
      context.useModel().addElement(xmlpath).addData(ropeOutput, AXmlElement::ENC_CDATADIRECT);
    }
  }

  return AOSContext::RETURN_OK;
}

