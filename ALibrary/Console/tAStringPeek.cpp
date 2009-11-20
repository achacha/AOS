#include "AString.hpp"
#include "AException.hpp"

int main(int argc, char **argv)
{
  try
  {
    AString str("0123456789");
    AString target;

    str.peek(target, 0, 20);

    std::cout << target << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }

  return 0;
}
