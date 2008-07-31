/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Test.hpp"
#include "AOSModule_SessionTest.hpp"

const AString AOSModule_SessionTest::S_COUNT("count");
const AString AOSModule_SessionTest::S_SESSIONCREATED("test/sessionCreated");
const AString AOSModule_SessionTest::S_OUTPATH("test/count");

const AString& AOSModule_SessionTest::getClass() const
{
  static const AString CLASS("SessionTest");
  return CLASS;
}

AOSModule_SessionTest::AOSModule_SessionTest(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_SessionTest::execute(AOSContext& context, const AXmlElement& params)
{
  AString str(128, 256);

  if (context.existsSessionData())
  {
    if (context.getSessionDataContent(S_COUNT, str))
    {
      ANumber n(str);
      ++n;
      str.clear();
      n.emit(str);
      context.useSessionData().useData().setString(S_COUNT, str);
      context.useModel().setString(S_OUTPATH, str);
    }
    else
    {
      //a_Session exists add counter
      context.useSessionData().useData().setString(S_COUNT, AConstant::ASTRING_ZERO);
      context.useModel().setString(S_OUTPATH, AConstant::ASTRING_ZERO);
    }
  }
  else
  {
    //a_Add new sesson and counter
    context.useSessionData().useData().setString(S_COUNT, AConstant::ASTRING_ZERO);
    context.useModel().addElement(S_SESSIONCREATED);
    context.useModel().setString(S_OUTPATH, AConstant::ASTRING_ZERO);
  }

  return AOSContext::RETURN_OK;
}
