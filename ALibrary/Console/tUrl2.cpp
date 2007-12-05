#include "AUrl.hpp"

void test_1()
{
  AUrl url("object:///namespace/object.subset");
  
  std::cout << "Initial namespace: " << url.getBaseDirName() << std::endl;
  url.setPath("global");
  std::cout << "Global namespace: " << url.getBaseDirName() << std::endl;

  AUrl urlA(url), urlB(url);
  AUrl urlNew("Object:///newpath/newobject.sub2");
  urlA &= urlNew;
  urlB |= urlNew;
  std::cout << "Result of &: " << urlA << std::endl;
  std::cout << "Result of |: " << urlB << std::endl;
}

void test_debugDump()
{
  AUrl url("object:///namespace/object.subset?y=1&z=2");
  url.debugDump(std::cout, 0);
}

void test_Ref()
{
  AUrl url("http://myserver.com/path/file#reference");
  url.debugDump();
}

int main()
{
  //test_1();
  //test_debugDump();
  test_Ref();

  return 0x0;
}
