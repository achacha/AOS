#include "ATime.hpp"

int main()
{
  u8 count0 = ATime::getHighPerfCounter();
  u8 freq = ATime::getHighPerfFrequency();
  u8 count1 = ATime::getHighPerfCounter();

  double diff = (double(count1) - double(count0)) / freq;

  std::cout << diff << std::endl;
}