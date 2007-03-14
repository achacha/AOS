#include <AObject.hpp>

void testOutput()
{
  AObject obj;

  obj.set("a", "1");
  obj.set("a", "2");
  obj.set("a", "3");
  obj.set("b", "0");
  obj.set("x", "123");
  obj.set("y", "456");

  cout << obj.toString().c_str() << endl;
  cout << obj.toJavascript().c_str() << endl;
}

int main()
{
  testOutput();

  return 0x0;
}
