#include "pchAOS_User.hpp"
#include "AOSModule_AuthenticateUser.hpp"

const AString& AOSModule_AuthenticateUser::getClass() const
{
  static const AString CLASS("AuthenticateUser");
  return CLASS;
}

AOSModule_AuthenticateUser::AOSModule_AuthenticateUser(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_AuthenticateUser::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  return true;
}
