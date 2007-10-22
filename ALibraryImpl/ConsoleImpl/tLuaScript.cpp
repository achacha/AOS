#include "ALuaEmbed.hpp"
#include "ATemplate.hpp"
#include "ABasePtrHolder.hpp"
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
    AFilename fn(ASWNL(argv[1]), false);
    AFile_Physical scriptfile(fn);
    scriptfile.open();

    ABasePtrHolder objects;
    objects.insert(ATemplate::OBJECTNAME_MODEL, new AXmlDocument("root"), true);

    AString output(10240, 4096);
    ALuaEmbed lua(objects, output);
    
    ARope script;
    scriptfile.emit(script);
    lua.execute(script);

    std::cout << output << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }

  return 0;
}