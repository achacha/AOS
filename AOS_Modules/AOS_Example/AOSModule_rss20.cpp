#include "pchAOS_Example.hpp"
#include "AOSModule_rss20.hpp"
#include "ADatabase.hpp"
#include "AResultSet.hpp"

const AString& AOSModule_rss20::getClass() const
{
  static const AString CLASS("rss20_sample");
  return CLASS;
}

AOSModule_rss20::AOSModule_rss20(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_rss20::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& output = context.useModel();
  
  output.useName().assign("rss");
  output.addAttribute("version", "2.0");

  //a_Channel info
  AXmlElement& channel = output.addElement("channel");
  channel.addElement("title").addData("AObjectServer Home");
  channel.addElement("language").addData("en-us");
  channel.addElement("link").addData("http://www.achacha.org/");
  channel.addElement("description").addData("AObjectServer Homepage");
    
  //a_Items
  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("The dada raven");
    item.addElement("link").addData("http://achacha.selfip.com/dada/Dada?templateName=TheRaven");
    item.addElement("description").addData("Nevermore");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("The dada freeflow");
    item.addElement("link").addData("http://achacha.selfip.com/dada/Dada?templateName=FreeFlow");
    item.addElement("description").addData("Free flow of information");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("Template: Single");
    item.addElement("link").addData("http://achacha.selfip.com/DisplayInputWithTemplate.html");
    item.addElement("description").addData("Single template");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("Template: Multiple");
    item.addElement("link").addData("http://achacha.selfip.com/DisplayInputWithTemplateParts.html");
    item.addElement("description").addData("Multi-part template composite");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("XSLT");
    item.addElement("link").addData("http://achacha.selfip.com/DisplayInputWithXslt.html");
    item.addElement("description").addData("XSL transform");
  }

  return AOSContext::RETURN_OK;
}

