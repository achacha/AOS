#include <float.h>
#include <math.h>
#include "ATimer.hpp"
#include "ANumber.hpp"

#define MAX_ITER 9999999

void testFloat()
{
  std::cout << "Starting 'float' test:" << std::flush;
  ATimer timer;
  float x0 = 13.0, x1 = 231.0, xr;

  timer.start();
  for (int t=0; t<MAX_ITER; ++t)
  {
    xr = sqrt(x0 * x0 + x1 * x1);
  }
  u4 tx = timer.getInterval();
  std::cout << " duration=" << tx << std::endl;
}

void testDouble()
{
  std::cout << "Starting 'double' test:" << std::flush;
  ATimer timer;
  double x0 = 13, x1 = 231, xr;

  timer.start();
  for (int t=0; t<MAX_ITER; ++t)
  {
    xr = sqrt(x0 * x0 + x1 * x1);
  }
  u4 tx = timer.getInterval();
  std::cout << " duration=" << tx << std::endl;
}

int main()
{
  testFloat();
  testDouble();

  return 0;
}
