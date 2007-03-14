#include <iostream>
#include <windows.h>

void doDouble(double x)
{
  double a = 1.0 - x;
  double w;
  bool sign = true;
  int i,j;
  for (i=2; i<200; ++i)
  {
    w = 1.0;
    for (j=i; j>0; --j)
    {
      w *= (x / i);
    }
    if (sign)
      a += w;
    else
      a -= w;
    sign = !sign;
  }
}

void doFloat(float x)
{
  float a = 1.0f - x;
  float w;
  bool sign = true;
  int i,j;
  for (i=2; i<200; ++i)
  {
    w = 1.0;
    for (j=i; j>0; --j)
    {
      w *= (x / i);
    }
    if (sign)
      a += w;
    else
      a -= w;
    sign = !sign;
  }
}

int main()
{
  DWORD startTick;
  DWORD endTick;
  ::srand(::GetTickCount());

  {
    startTick = ::GetTickCount();
    for (int i=0; i<10000; ++i)
    {
      double x = double(::rand()) / RAND_MAX * 3.14159654;
      doDouble(x);
    }

    endTick = ::GetTickCount();
    std::cout << "double time: " << endTick - startTick << "ms" << std::endl;
  }

  {
    startTick = ::GetTickCount();
    for (int i=0; i<10000; ++i)
    {
      float x = float(::rand()) / RAND_MAX * 3.14159654f;
      doFloat(x);
    }

    endTick = ::GetTickCount();
    std::cout << "float time: " << endTick - startTick << "ms" << std::endl;
  }


  return 0;
}