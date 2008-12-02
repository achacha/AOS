
#include "AOS.hpp"
#include "AGdCanvas.hpp"

#ifndef NDEBUG
#include "MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif

int main()
{
  AGdCanvas canvas(100, 100);
  canvas.setPixel(50, 50, gdTrueColor(200,100,250));

  return 0;
}