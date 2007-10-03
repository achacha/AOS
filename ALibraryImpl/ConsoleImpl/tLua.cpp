#include "AString.hpp"
#include "ALuaEmbed.hpp"
#include "AException.hpp"
#include "AFile_AString.hpp"

int main()
{
  try
  {
    AFile_AString strf(
"\
print '<b>Hello World</b><br/><small>';\n\
alibrary.foo();\n\
print '</small>';\n\
");
    
    ALuaEmbed lua(ALuaEmbed::LUALIB_ALL);
    lua.execute(strf);
    std::cout << lua.useOutputBuffer() << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }

  return 1;
}