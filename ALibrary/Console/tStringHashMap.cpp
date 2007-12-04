#include "AStringHashMap.hpp"
#include "AFile_AString.hpp"

int main()
{
  AStringHashMap h;

  h.set("&foo&bar1", "something 1");
  h.set("foo+bar0:", "something 0");
  h.set("/foo\bar2", "something 2");
  h.set(" foo bar ", "something 3");
  h.set("id", "1234567890");

  AFile_AString file;
  h.toAFile(file);
  file.debugDump();

  AStringHashMap hh;
  AStringHashMap hhh;
  hh.fromAFile(file);
  file.reset();
  hhh.fromAFile(file);

  std::cout << h << std::endl;
  std::cout << hh << std::endl;

  AString str;
  h.get("&foo&bar1", str);
  std::cout << str << std::endl;

  str.clear();
  h.get("/foo\bar2", str);
  std::cout << str << std::endl;

  return 0;
}