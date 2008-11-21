/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Example.hpp"
#include "AOSModule_rss20.hpp"

const AString& AOSModule_rss20::getClass() const
{
  static const AString CLASS("Example.rss20");
  return CLASS;
}

AOSModule_rss20::AOSModule_rss20(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_rss20::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& output = context.useModel();
  
  // Change root to rss
  output.useName().assign("rss");
  output.addAttribute("version", "2.0");

  // Channel info
  AXmlElement& channel = output.addElement("channel");
  channel.addElement("title").addData("AObjectServer Home");
  channel.addElement("language").addData("en-us");
  channel.addElement("link").addData("http://www.achacha.org/aos/");
  channel.addElement("description").addData("AObjectServer Homepage");
    
  // Items
  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("AOS at Microsoft CodePlex");
    item.addElement("link").addData("http://www.codeplex.com/aobjectserver");
    item.addElement("description").addData("Home of AOS on Microsoft CodePlex");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("AOS at SourceForge");
    item.addElement("link").addData("http://sourceforge.net/projects/aobjectserver/");
    item.addElement("description").addData("Home of AOS on SourceForge");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("Microsoft Visual Studio");
    item.addElement("link").addData("http://www.microsoft.com/express/vc/");
    item.addElement("description").addData("Microsoft Visual Studio C++ Express Homepage and Free Download");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("Lua script");
    item.addElement("link").addData("http://www.lua.org/");
    item.addElement("description").addData("Lua programming language homepage");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("SQLite3 database");
    item.addElement("link").addData("http://www.sqlite.org/");
    item.addElement("description").addData("SQLIte homepage");
  }

  {
    AXmlElement& item = channel.addElement("item");
    item.addElement("title").addData("MySQL database");
    item.addElement("link").addData("http://www.mysql.com/");
    item.addElement("description").addData("MySQL homepage");
  }

  return AOSContext::RETURN_OK;
}

