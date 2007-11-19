#include "pchAOS_Forum.hpp"
#include "AOSModule_forum_list.hpp"

const AString& AOSModule_forum_list::getClass() const
{
  static const AString CLASS("forum_list");
  return CLASS;
}

AOSModule_forum_list::AOSModule_forum_list(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSModule_forum_list::~AOSModule_forum_list()
{
}

bool AOSModule_forum_list::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& eForums = context.useModel().addElement("forums");

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name").addData("General");
    eForum.addElement("message_count").addData("130");
  }
  
  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name").addData("Comments");
    eForum.addElement("message_count").addData("72");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name").addData("Questions");
    eForum.addElement("message_count").addData("28");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name").addData("Suggestions");
    eForum.addElement("message_count").addData("32");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name").addData("Out Of Context");
    eForum.addElement("message_count").addData("112");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name").addData("Rants");
    eForum.addElement("message_count").addData("9");
  }

  return true;
}
