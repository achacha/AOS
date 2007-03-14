#include <templateAPathMap.hpp>
#include <AString.hpp>

#undef main
int main()
{
  APathMap pm("D:/test000.ini");

  pm.setValue("/Section0", "item00", "value00", FALSE);
  pm.setValue("/Section0", "item01", "value01", FALSE);
  pm.setValue("/Section1", "item10", "value10", FALSE);
  pm.writeNow();

  return 0x0;
}
