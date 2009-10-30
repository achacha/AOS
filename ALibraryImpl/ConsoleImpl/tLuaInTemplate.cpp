#include "apiABase.hpp"
#include "apiALuaEmbed.hpp"
#include "ALuaTemplateContext.hpp"
#include "ALuaEmbed.hpp"
#include "AXmlDocument.hpp"
#include "AFilename.hpp"
#include "AFile_Physical.hpp"
#include "ASocketLibrary_SSL.hpp"

ASocketLibrary_SSL g_SocketLibrarySSL;

int main(int argc, char **argv)
{
//  AString code0(
//"result=web.HttpGet(\"test\", \"http://localhost/ping\");\
//print result;\
//");

  if (argc < 2)
  {
    std::cerr << argv[0] << " <lua script file>\r\n" << std::endl;
    return -1;
  }

  AFilename f(ASWNL(argv[1]), false);
  std::cout << "Loading: " << f << std::endl;

  ABasePtrContainer *pobjects = new ABasePtrContainer();
  AString output;
  try
  {
    AFile_Physical file(f);
    file.open();

    AXmlDocument model;
    model.useRoot().useName().assign("root");

    AEventVisitor visitor;

    ALuaTemplateContext ctx(*pobjects, model, visitor);

    ALuaEmbed lua;
  
    std::cout << "Executing lua script... " << f << std::endl;
    lua.execute(file, ctx, output);

    std::cout << "\r\n------------------------------------ output -----------------------------------------" << std::endl;
    std::cout << output << std::endl;
    std::cout << "------------------------------------ output -----------------------------------------" << std::endl;

    delete pobjects;
  }
  catch(AException& ex)
  {
    std::cout << ex << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception." << std::endl;
  }
  
  return 1;
}