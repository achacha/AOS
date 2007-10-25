#include "pchAOS_Wiki.hpp"
#include "AOSModule_WikiView.hpp"

const AString& AOSModule_WikiView::getClass() const
{
  static const AString CLASS("WikiView");
  return CLASS;
}

AOSModule_WikiView::AOSModule_WikiView(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_WikiView::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  
  return true;
}
