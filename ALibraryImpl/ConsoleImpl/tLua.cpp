#include "AString.hpp"
#include "ALuaEmbed.hpp"
#include "AException.hpp"

int main()
{
  try
  {
    ALuaEmbed lua;
    lua.execute(ASWNL("print(Hello World)\r\n"));
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }
}