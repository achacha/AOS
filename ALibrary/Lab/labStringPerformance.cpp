#include "AString.hpp"

void testStripTrailing()
{
  AString str("TEST5     ");
  str.stripTrailing();
}

int main()
{
  testStripTrailing();
  return 0;
}