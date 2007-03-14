#include "AOSSessionManager.hpp"
#include "AOSSessionData.hpp"

int main()
{
  AOSSessionManager sm;
  sm.debugDump();
  AOSSessionData& d1 = sm.getSessionData("1234");
  AOSSessionData& d2 = sm.getSessionData("1235");
  sm.debugDump();

  std::cout << "\r\n\r\n";

  d1.useObjects().insert("foo", "1");
  d1.useObjects().insert("bar", "2");

  d1.debugDump();
  AOSSessionData& d3 = sm.getSessionData("1234");
  d3.debugDump();

  sm.debugDump();

  return 0x0;
}
