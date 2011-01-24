#include "ATime.hpp"
#include <stdio.h>
#include "AException.hpp"

int main()
{
  try
  {
    ATime time1;
    ATime time2(time1);

    if (time1 == time2)
      std::cout << "Equals!" << std::endl;
    else
      std::cout << "Not equals!" << std::endl;
  }
  catch(AException& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0x0;
}
