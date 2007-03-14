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
  std::cout << "Result of &: " << urlA.toString() << std::endl;
  std::cout << "Result of |: " << urlB.toString() << std::endl;
}

void test_debugDump()
{
  AUrl url("object:///namespace/object.subset?y=1&z=2");
  url.debugDump(std::cout, 0);
}

void test_PathParts()
{
  AUrl url("path0/path1/path2/name");
  int i = 0;
  LIST_AString sv;
  url.getPathParts(sv);
  while (sv.size() > 0)
  {
    std::cout << i++ << ": " << sv.front() << std::endl;
    sv.pop_front();
  }

  url.parse("onlyname");
  url.getPathParts(sv);
  i=0;
  while (sv.size() > 0)
  {
    std::cout << i++ << ": " << sv.front() << std::endl;
    sv.pop_front();
  }

  url.parse("/root/sub0/sub1/");
  url.getPathParts(sv);
  LIST_AString::iterator it = sv.begin();
  i=0;
  while (it != sv.end())
  {
    std::cout << i++ << ": " << (*it) << std::endl;
    ++it;
  }

}

int main()
{
  //test_1();
  //test_debugDump();
  test_PathParts();

  return 0x0;
}
