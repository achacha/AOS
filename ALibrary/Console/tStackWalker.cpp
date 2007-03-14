#include "AString.hpp"
#include "AStackWalker.hpp"

int main()
{
//  AString str;
  AStackWalker sw(AStackWalker::SWO_SET_STACKONLY);
  sw.ProcessCallstack();

  std::cout << sw << std::endl;
  
  return 0;
}