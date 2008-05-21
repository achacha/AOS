#include "apiABase.hpp"
#include "apiALuaEmbed.hpp"
#include "ALuaTemplateContext.hpp"
#include "ALuaEmbed.hpp"
#include "AXmlDocument.hpp"

int main()
{
  AString code(
"result=web.HttpGet(\"http://192.168.0.128/iw-cc/command/iw.ui\");\
print result;\
");

  ABasePtrContainer objects;
  AXmlDocument model("root");
  
  ALuaTemplateContext ctx(objects, model);

  AString output;
  ALuaEmbed lua;
  
  try
  {
    lua.execute(code, ctx, output);
  }
  catch(AException& ex)
  {
    std::cout << ex << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception." << std::endl;
  }

  std::cout << "\r\n------------------------------------ output -----------------------------------------" << std::endl;
  std::cout << output << std::endl;
  std::cout << "\r\n------------------------------------ output -----------------------------------------" << std::endl;

  return 1;
}