#include "templateACache.hpp"
#include "AFile_IOStream.hpp"
#include "AUrl.hpp"

typedef ACache<AString, AUrl> CACHE;

int main()
{
//  AFile_IOStream iosfile;
  
  CACHE cache;
  
  cache["url1"] = AUrl("http://www.achacha.org/aos.html");
  cache["url2"] = AUrl("http://www.achacha.org/index.html");

  cache.debugDump();

  return 0;
}
