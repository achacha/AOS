#include <iostream>
#include <limits.h>

int main()
{
  int userEntry = 0;
  int highest = -INT_MAX;
  int lowest = INT_MAX;
  while (true)
  {
    std::cout << "Please enter a number:" << std::flush;
    std::cin >> userEntry;
    
    if (-99 == userEntry)
      break;

    if (userEntry > highest)
      highest = userEntry;
    if (userEntry < lowest)
      lowest = userEntry;
  }

  std::cout << "Highest=" << highest << "  Lowest=" << lowest << std::endl;
  return 0;
}
