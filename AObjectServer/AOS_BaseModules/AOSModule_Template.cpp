/*
Written by Alex Chachanashvili

$Id$
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
  AAutoPtr<ATemplate> pTemplate(NULL, false);
  AAutoPtr<AFile> pFile(NULL, false);
  if (pNode)
  {
    //a_Element contains script
    pTemplate.reset(m_Services.createTemplate(), true);
    
    //a_Parse template
    AFile_AString strfile;
    pNode->emitContent(strfile);
    pTemplate->fromAFile(strfile);
  }
  else
  {
    //a_Filename provided, use the cache
    pNode = params.findElement(AOS_BaseModules_Constants::FILENAME);
    if (pNode)
    {
      AString relativePath;
      pNode->emitContent(relativePath);

      //a_File to be used (may need caching for it, but for now keep it dynamic)
      AFilename filename(m_Services.useConfiguration().getAosBaseDataDirectory(), true);
      filename.join(relativePath, false);
      if (ACacheInterface::NOT_FOUND == m_Services.useCacheManager().getTemplate(context, filename, pTemplate))
      {
        //a_Not found, return error
        ARope rope;
        rope.append(getClass());
        rope.append(": Unable to find a template file: ",34);
        rope.append(filename);
        context.useEventVisitor().startEvent(rope, AEventVisitor::EL_ERROR);
        return AOSContext::RETURN_ERROR;
      }
    }
    else
    {
      context.addError(getClass(), ASWNL("Unable to find module/template nor module/filename, Template module did not execute, params"));
      return AOSContext::RETURN_ERROR;  //a_Did not find either module/template or module/filename
    }
  }  

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
  pNode = params.findElement(AOS_BaseModules_Constants::PATH);
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
  else
  {
    context.useEventVisitor().addEvent(ASWNL("Unable to find module/path, output from template discarded"), AEventVisitor::EL_WARN);
  }
  return AOSContext::RETURN_OK;
}

