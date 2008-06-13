#include "AOS.hpp"
#include "ATemplateNodeHandler_LUA.hpp"
#include "ATemplateNodeHandler_SESSION.hpp"

int main()
{
  AFile_AString tdata("Hello %[SESSION]{{{some/path}}}[SESSION]%");

  AAutoPtr<ATemplate> pTemplate(new ATemplate(), true);
  
  pTemplate->addHandler(new ATemplateNodeHandler_LUA());
  pTemplate->addHandler(new ATemplateNodeHandler_SESSION());

  pTemplate->fromAFile(tdata);
  
  AXmlDocument model("root");
  ABasePtrContainer objects;
  ATemplateContext ctx(objects, model);

  pTemplate->debugDump();
  
  AString str;
  pTemplate->process(ctx, str);

  return 0;
}