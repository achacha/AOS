#include "pchAOS_Example.hpp"
#include "AOSModule_rss20.hpp"
#include "ADatabase.hpp"
#include "AResultSet.hpp"

const AString& AOSModule_rss20::getClass() const
{
  static const AString CLASS("rss20_sample");
  return CLASS;
}

AOSModule_rss20::AOSModule_rss20(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_rss20::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& output = context.useOutputRootXmlElement();
  
  output.useName().assign("rss");
  output.addAttribute("version", "2.0");

  //a_Channel info
  AXmlElement& channel = output.addElement("channel");
  channel.addElement("title", "AObjectServer Home");
  channel.addElement("language", "en-us");
  channel.addElement("link", "http://www.achacha.org");
  channel.addElement("description", "AObjectServer Homepage");
    
  //a_Items
  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title", "The dada raven");
    item.addElement("link", "http://achacha.selfip.com/dada/Dada?templateName=TheRaven");
    item.addElement("description", "Nevermore");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title", "The dada freeflow");
    item.addElement("link", "http://achacha.selfip.com/dada/Dada?templateName=FreeFlow");
    item.addElement("description", "Free flow of information");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title", "Template: Single");
    item.addElement("link", "http://achacha.selfip.com/DisplayInputWithTemplate.html");
    item.addElement("description", "Single template");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title", "Template: Multiple");
    item.addElement("link", "http://achacha.selfip.com/DisplayInputWithTemplateParts.html");
    item.addElement("description", "Multi-part template composite");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title", "XSLT");
    item.addElement("link", "http://achacha.selfip.com/DisplayInputWithXslt.html");
    item.addElement("description", "XSL transform");
  }

  return true;
}

