#include "AFragmentString.hpp"

int main()
{
  AFragmentString fragment;
  fragment.parse(ASWNL(".<2>."));
  fragment.debugDump();

  std::cout << "Permutations=" << fragment.getPermutations() << std::endl;

  AString str;
  while (fragment.next(str))
  {
    std::cout << str << ", " << std::flush;
    str.clear();
  }
  std::cout << str << std::endl;

  fragment.debugDump();

  return 0;
}