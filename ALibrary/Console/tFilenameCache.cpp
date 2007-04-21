#include "AFilename.hpp"
#include "AString.hpp"
#include "templateACache.hpp"
#include "ARope.hpp"

int main()
{
  ACache<AFilename, AString> cache;

  AFilename f0("c:\\foo\\bar.txt");
  AFilename f1("c:/foo/bar.zip");
  
  cache[f0].assign("Text file");
  cache[f1].assign("Zip file");

  for (ACache<AFilename, AString>::iterator it = cache.begin(); it != cache.end(); ++it)
  {
    std::cout << (*it).first << "=" << (*it).second << std::endl;
  }

  AXmlElement xml("root");
  cache.emit(xml);
  ARope rope;
  xml.emit(rope);
  rope.append("\r\n",2);
  cache.emit(rope);
  std::cout << "\r\n" << rope << "\r\n" << std::endl;

  return 0;
}              