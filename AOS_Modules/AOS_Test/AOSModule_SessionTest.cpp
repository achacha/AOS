#include "pchAOS_Test.hpp"
#include "AOSModule_SessionTest.hpp"

const AString& AOSModule_SessionTest::getClass() const
{
  static const AString CLASS("SessionTest");
  return CLASS;
}

AOSModule_SessionTest::AOSModule_SessionTest(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_SessionTest::execute(AOSContext& context, const AXmlElement& params)
{
  static const AString COUNT("count");
  static const AString OUTPATH("test/count");
  AString str(128, 256);

  //a_Session counter
  AXmlElement& sessionData = context.useSessionData().useData();
  if (sessionData.emitFromPath(COUNT, str))
  {
    ANumber n(str);
    ++n;
    str.clear();
    n.emit(str);
    sessionData.setString(COUNT, str);
    context.useModel().setString(OUTPATH, str);
  }
  else
  {
    sessionData.setString(COUNT, AConstant::ASTRING_ZERO);
    context.useModel().setString(OUTPATH, AConstant::ASTRING_ZERO);
  }

  return true;
}
