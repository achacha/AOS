#include <iostream.h>
#include "AMovingAverage.hpp"
#include "ARandomNumberGenerator.hpp"

int main()
{
  AMovingAverage a;
  const int iMax = 100;
  
  int iVal;  
  long lX = 0x0;
  for (int iX = 0x0; iX < iMax; ++iX)
  {
    iVal = ARandomNumberGenerator::intRange(100);
    lX += iVal;
    a.addSample(iVal);
    cerr << iVal << endl;
  }

  lX /= iX;

  a.debugDump(cerr);
  cerr << "Mean=" << lX << endl;
  
  return 0x0;
}
