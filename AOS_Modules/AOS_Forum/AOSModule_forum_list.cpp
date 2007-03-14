#include "pchAOS_Forum.hpp"
#include "AOSModule_forum_list.hpp"

const AString& AOSModule_forum_list::getClass() const
{
  static const AString CLASS("forum_list");
  return CLASS;
}

AOSModule_forum_list::AOSModule_forum_list(ALog& alog) :
  AOSModuleInterface(alog)
{
}

AOSModule_forum_list::~AOSModule_forum_list()
{
}

bool AOSModule_forum_list::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& eForums = context.useOutputRootXmlElement().addElement("forums");

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name", "General");
    eForum.addElement("message_count", "130");
  }
  
  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name", "Comments");
    eForum.addElement("message_count", "72");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name", "Questions");
    eForum.addElement("message_count", "28");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name", "Suggestions");
    eForum.addElement("message_count", "32");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name", "Out Of Context");
    eForum.addElement("message_count", "112");
  }

  {
    AXmlElement& eForum = eForums.addElement("forum");
    eForum.addElement("name", "Rants");
    eForum.addElement("message_count", "9");
  }

  return true;
}
