#include <AObject.hpp>

int main()
{
  AObject obj("object:///namespace/object.subset?y=1&z=2");
  obj.debugDump(std::cout, 0);

  return 0x0;
}
