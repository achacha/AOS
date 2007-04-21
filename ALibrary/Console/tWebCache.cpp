#include "ACache_FileSystem.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"

int main()
{
  ACache_FileSystem cache;

  cache.get(AFilename("C:/Code/_debug/aos_root/static/index.html"));
  cache.get(AFilename("C:/Code/_debug/aos_root/static/login_md5.html"));
  cache.get(AFilename("C:/Code/_debug/aos_root/static/aos.html"));
  cache.get(AFilename("C:/Code/_debug/aos_root/static/donotexist.foo"));
  cache.get(AFilename("C:/Code/_debug/aos_root/static/login_md5.html"));
  cache.get(AFilename("C:/Code/_debug/aos_root/static/aos.html"));
  cache.get(AFilename("C:/Code/_debug/aos_root/static/donotexist.foo"));

  AFile *pFile = cache.get(AFilename("C:/Code/_debug/aos_root/static/index.html"));
  ARope rope;
  if (pFile)
    pFile->emit(rope);
  std::cout << rope << std::endl;

  rope.clear();
  AXmlElement xml;
  cache.emit(xml);
  xml.emit(rope, 0);
  std::cout << rope << std::endl;

  rope.clear();
  cache.emit(rope);
  std::cout << rope << std::endl;

  return 0;
}