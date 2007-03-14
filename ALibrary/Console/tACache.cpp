#include "ACache.hpp"
#include "AFile_IOStream.hpp"
#include "AMovingAverage.hpp"
#include "AUrl.hpp"
#include "ACriticalSection.hpp"

int main()
{
  AFile_IOStream iosfile;
  ACache cache(new ACriticalSection());
  
  cache.set("name", AString("Foo"));
  cache.set("url", AUrl("http://www.achacha.org/"));
  cache.set("avg", AMovingAverage());

  cache.debugDump();

  return 0;
}
