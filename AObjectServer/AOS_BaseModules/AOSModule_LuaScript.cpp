#include "pchAOS_BaseModules.hpp"
#include "AOSModule_LuaScript.hpp"
#include "AFile_AString.hpp"
#include "AFile_Physical.hpp"
#include "ABasePtrHolder.hpp"
#include "AXmlElement.hpp"
#include "ATemplateNodeHandler_LUA.hpp"

const AString& AOSModule_LuaScript::getClass() const
{
  static const AString MODULE_CLASS("LuaScript");
  return MODULE_CLASS;
}

AOSModule_LuaScript::AOSModule_LuaScript(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_LuaScript::execute(AOSContext& context, const AXmlElement& params)
{
  static const AString OUTPUT("output",6);

  const AXmlElement *pNode = params.findElement(ASW("script",6));
  AString strSource;
  AAutoPtr<AFile> pFile;
  if (pNode)
  {
    strSource.assign("inline", 6);
    context.setExecutionState(ASW("Executing inlined Lua script",28));

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
      pNode->emitContent(strSource);
      context.setExecutionState(ARope("Executing Lua script: ",22)+strSource);

      //a_File to be used (may need caching for it, but for now keep it dynamic)
      AFilename f(m_Services.useConfiguration().getAosBaseDataDirectory(), true);
      f.join(strSource, false);
      
      pFile.reset(new AFile_Physical(f));
      pFile->open();
    }
    else
    {
      context.addError(ASWNL("AOSModule_LuaScript"), ASWNL("Unable to find module/script nor module/filename, LuaScript module did not execute"));
      return false;  //a_Did not find either module/script or module/filename
    }
  }
  
  //a_Objects
  ABasePtrHolder objects;
  objects.insert(ATemplate::OBJECTNAME_MODEL, &context.useModelXmlDocument());  //a_Add ALibrary required model AXmlDocument
  objects.insert(AOSContext::OBJECTNAME, &context);                              //a_Add AOS required AOSContext

  //a_Process and save output
  ARope ropeOutput;
  AAutoPtr<ATemplate> pTemplate(m_Services.createTemplate());

  //a_Read script and insert nodes into template
  pTemplate->addNode(ATemplateNodeHandler_LUA::TAGNAME, *pFile);

  //a_Process template
  pTemplate->process(objects, ropeOutput);
  
  //a_Insert output into outpath (if any)
  pNode = params.findElement(ASW("outpath",7));
  if (pNode)
  {
    AString xmlpath;
    pNode->emitContent(xmlpath);
    if (!xmlpath.isEmpty())
    {
      //a_Add output as CDATA
      AXmlElement& eOutput = context.useModel().overwriteElement(xmlpath).addElement(OUTPUT);
      eOutput.addData(ropeOutput, AXmlElement::ENC_CDATADIRECT);
      eOutput.addAttribute(ASW("source",6), strSource);
    }
  }

  //a_Add template to debug
  if (context.getDumpContextLevel() > 0)
  {
    AString str("debug/",6);
    str.append(getClass());
    str.append("/script",7);
    AXmlElement& base = context.useModel().addElement(str);
    pTemplate->emitXml(base);
    base.addAttribute(ASW("source",6), strSource);
  }

  return true;
}

