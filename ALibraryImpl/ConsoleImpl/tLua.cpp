#include "AString.hpp"
#include "ALuaEmbed.hpp"
#include "AException.hpp"

int main()
{
  try
  {
    ALuaEmbed lua(ALuaEmbed::LUALIB_ALL);
    lua.execute(ASWNL("print \"Hello World\";\r\nalibrary.foo();\r\n"));
    std::cout << lua.useOutputBuffer() << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }

  return 1;
}