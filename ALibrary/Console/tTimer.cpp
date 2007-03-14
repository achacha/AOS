#include "ATimer.hpp"
#include "AThread.hpp"

int main()
{
  ATimer t(true);
  AThread::sleep(1000);
  double diff = t.getInterval();
  std::cout << diff << std::endl;

  return 0;
}
