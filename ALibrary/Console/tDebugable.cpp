#include "AString.hpp"
#include "ARope.hpp"

int main()
{
  ARope debugResult;
  ARope rope(32);

  rope.append("A lazy crazy box jumped over the brown bog.");
  rope.append("Oh that too too palid flesh would wilt and melt resolve itself into a mountain dew.");

  rope.debugDumpToAOutputBuffer(debugResult);
  std::cout << debugResult.toAString() << std::endl;
  
  debugResult.debugDump();

  return 0;
}
