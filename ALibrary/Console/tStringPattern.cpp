#include "AString.hpp"

int main()
{
  AString base("HTTP/1.0 200 Ok?");

  std::cout << "'*/1.? 200' = " << (base.matchPattern(ASWNL("*/1.? 200")) ? "found" : "not found") << std::endl;
  std::cout << "'*200' = " << (base.matchPattern(ASWNL("*200")) ? "found" : "not found") << std::endl;
  std::cout << "'H??P' = " << (base.matchPattern(ASWNL("H??P")) ? "found" : "not found") << std::endl;
  std::cout << "'??' = " << (base.matchPattern(ASWNL("??")) ? "found" : "not found") << std::endl;
  std::cout << "'*' = " << (base.matchPattern(ASWNL("*")) ? "found" : "not found") << std::endl;
  std::cout << "'*\\?' = " << (base.matchPattern(ASWNL("*\\?")) ? "found" : "not found") << std::endl;

  std::cout << "'?f' = " << (base.matchPattern(ASWNL("?f")) ? "found" : "not found") << std::endl;
  std::cout << "'*z' = " << (base.matchPattern(ASWNL("*z")) ? "found" : "not found") << std::endl;

  return 0;
}