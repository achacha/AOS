#include "ALuaEmbed.hpp"
#include "ALuaTemplateContext.hpp"
#include "ATemplate.hpp"
#include "ABasePtrContainer.hpp"
#include "AFilename.hpp"
#include "AFile_Physical.hpp"
#include "ASocketLibrary.hpp"

ASocketLibrary g_SocketLibrary;

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: this <Lua script filename>" << std::endl;
    return -1;
  }
  
  try
  {
    // Build lua execution context
    ABasePtrContainer objects;
    AXmlDocument model;
    AEventVisitor visitor;
    ALuaTemplateContext luaContext(objects, model, visitor);    

    // Open script
    AFilename fn(ASWNL(argv[1]), false);
    AFile_Physical scriptfile(fn);
    scriptfile.open();

    // Execute and output
    AString output(10240, 4096);
    luaContext.execute(scriptfile, output);

    std::cout << output << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }

  return 0;
}