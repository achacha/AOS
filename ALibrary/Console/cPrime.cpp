#include <iostream>
#include "ANumber.hpp"
#include "ATimer.hpp"

void testPrimeTest()
{
  ANumber n(17);
  for (int i=0; i<60; ++i) {
    std::cout << n.toAString() << (n.isPrime() ? " is prime" : " is not prime") << std::endl;
    n += ANumber(10);
  }
}

void displayDigits()
{
  ANumber n(11);
  const ANumber N_ten(10);

  for (int i=0; i<20; ++i) {
    //a_Sum digits
    AString str = n.toAString();
    u4 uL = str.getSize() - 1;
    ANumber sum;
    u4 u=uL;
    do {
      sum += ANumber(AString(str.at(u)));
    } while(u-- > 0);

    std::cout << str.c_str() << "\t" << (n.isPrime() ? "." : "!") << "\t" << sum.toAString().c_str() << std::endl;

    n += N_ten;
  }
}

int main()
{
  ATimer timer(true);

  //testPrimeTest();
  displayDigits();

  std::cout << "Test took " << timer.getInterval() << "ms.  Press ENTER...";
  std::cin.get();

  return 0;
}
    

