#include "AString.hpp"

void testNew()
{
  AString *p = new AString("foo",3);
  p->append("bar",3);
  delete p;
  p = NULL;
}

int main()
{
  testNew();

  return 0;
}