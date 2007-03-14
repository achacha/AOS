#include "pchAOS_BaseModules.hpp"
#include "AOSModule_Test.hpp"
#include "AObjectDatabase.hpp"

const AString AOSModule_Test::MODULE_CLASS("Test", 4);
const AString& AOSModule_Test::getClass() const
{
  return MODULE_CLASS;
}

AOSModule_Test::AOSModule_Test(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_Test::execute(AOSContext& context, const AObjectContainer& params)
{
  AString str(1024, 256);
  params.emit(str);
  
  AXmlElement& e = context.useOutputRootXmlElement().addElement(ASW("/test/",6), str, AXmlData::CDataSafe);

  //a_Session counter
  str.clear();
  AStringHashMap& session = context.useSessionData();
  if (session.get(ASW("count",5), str))
  {
    ANumber n(str);
    ++n;
    str.clear();
    n.emit(str);
    session.set(ASW("count",5), str);
  }
  else
  {
    session.set(ASW("count",5), AString::sstr_Zero);
  }

  return true;
}
