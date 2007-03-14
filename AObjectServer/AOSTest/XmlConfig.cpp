#include "AXmlDocument.hpp"
#include "AFile_Physical.hpp"
#include "AOSCommand.hpp"
#include "AXmlElement.hpp"
#include "templateListOfPtrs.hpp"

int main()
{
  try
  {
    AFile_Physical configfile;
    if (configfile.open("q:\\aosconfig\\commands.xml"))
    {
      AListOfPtrs<AOSCommand> autolist;
      AOSCommand command;
      AXmlDocument doc;
      doc.fromAFile(configfile);
      AXmlElement::NodeContainer nodes;
      doc.findXPath("/commands/command", nodes);
      AXmlElement::NodeContainer::const_iterator cit = nodes.begin();
      while(cit != nodes.end())
      {
        AXmlElement *pElement = dynamic_cast<AXmlElement *>(*cit);
        if (pElement)
        {
          AOSCommand *p = new AOSCommand();
          p->fromAXmlElement(*pElement);
          autolist._list.push_back(p);
          p->debugDump();
        }
        ++cit;
      }
    }
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception caught." << std::endl;
  }
  return 0;
}