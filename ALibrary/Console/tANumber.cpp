#include <ANumber.hpp>
#include <ANMath.hpp>

int main()
{

//ANumber n;
//n.setPrecision(66);
//ANumber n0(n);

  cerr << "EulerNumber(1, 64) ==\r\n " << expN(ANumber(1), 64).toString() << " should be 2.718281828459..." << endl;

  return 0x0;
}