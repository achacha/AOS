#include "AFile_IOStream.hpp"
#include "ANumber.hpp"
#include "ATimer.hpp"
#include <math.h>
#include "AFile_Physical.hpp"

void testSqrtNumbers0()
{
  ANumber n0("3");
  n0.setPrecision(40);

  n0.sqrt();

  //a_1.7320508075688772935274463415059
  std::cout << "sqrt(3)=" << n0.toString() << std::endl;
  std::cout << "should =1.7320508075688772935274463415059" << std::endl;
  std::cout << "n0*n0  =" << (n0 * n0).toString() << std::endl;
  std::cout << std::endl;

  n0.setNumber("2");
  n0.setPrecision(40);
  n0.sqrt();

  //a_1.4142135623730950488016887242097
  std::cout << "sqrt(2)=" << n0.toString() << std::endl;
  std::cout << "should =1.4142135623730950488016887242097" << std::endl;
  std::cout << "n0*n0  =" << (n0 * n0).toString() << std::endl;
  std::cout << std::endl;

  n0.setNumber("3.1415965429");
  n0.setPrecision(40);
  n0.sqrt();

  //a_1.7724549480593293770975103015745
  std::cout << "sqrt(3.1415965429)=" << n0.toString() << std::endl;
  std::cout << "should =1.7724549480593293770975103015745" << std::endl;
  std::cout << "n0*n0  =" << (n0 * n0).toString() << std::endl;
  std::cout << std::endl;

  n0.setNumber("256");
  n0.setPrecision(40);
  n0.sqrt();

  //a_16
  std::cout << "sqrt(256)=" << n0.toString() << std::endl;
  std::cout << "should =16" << std::endl;
  std::cout << "n0*n0  =" << (n0 * n0).toString() << std::endl;
  std::cout << std::endl;


  n0.setNumber("12755271.1298371982379187");
  n0.setPrecision(40);
  n0.sqrt();

  //a_3571.4522438130400174943156771693
  std::cout << "sqrt(12755271.1298371982379187)=" << n0.toString() << std::endl;
  std::cout << "should =3571.4522438130400174943156771693" << std::endl;
  std::cout << "n0*n0  =" << (n0 * n0).toString() << std::endl;
  std::cout << std::endl;
}

void testSqrtNumbers1()
{
  ANumber n0("100");

  n0.sqrt();

  std::cout << "sqrt=" << n0.toString() << std::endl;
}

void testLoop(u4 iterations)
{
  std::cout << "Iteration count: " << iterations << std::endl;
  ATimer timer(true);
  for (u4 i=1; i<iterations; ++i)
  {
    ANumber n(i);
    n.setPrecision(10);

    double sqrt_c = sqrt(double(i));
    n.sqrt();
    n.forcePrecision();

    AString str;
    sprintf(str.startUsingCharPtr(128), "%10.10f", sqrt_c);
    str.stopUsingCharPtr();
    if (str != n.toString())
    {
      std::cout << "\nMismatch: " << i << ": " << str << " != " << n.toString() << std::endl;
      sprintf(str.startUsingCharPtr(128), "%10.15f", sqrt_c);
      str.stopUsingCharPtr();
      n = i;
      n.sqrt();
      std::cout << "\nLonger  : " << i << ": " << str << " != " << n.toString() << std::endl;
    }
    if (i % 100 == 0)
      std::cout << "." << std::flush;
  }
  u4 interval = timer.getInterval();
  std::cout << "\n\nElapsed time = " << interval << " ms " << std::endl;
}

void testSqrt2_1000()
{
  AString str1000digit(
    "1.4142135623730950488016887242096980785696718753769480731766797379907324784\
621070388503875343276415727350138462309122970249248360558507372126441214970\
999358314132226659275055927557999505011527820605714701095599716059702745345\
968620147285174186408891986095523292304843087143214508397626036279952514079\
896872533965463318088296406206152583523950547457502877599617298355752203375\
318570113543746034084988471603868999706990048150305440277903164542478230684\
929369186215805784631115966687130130156185689872372352885092648612494977154\
218334204285686060146824720771435854874155657069677653720226485447015858801\
620758474922657226002085584466521458398893944370926591800311388246468157082\
630100594858704003186480342194897278290641045072636881313739855256117322040\
245091227700226941127573627280495738108967504018369868368450725799364729060\
762996941380475654823728997180326802474420629269124859052181004459842150591\
120249441341728531478105803603371077309182869314710171111683916581726889419\
758716582152128229518488472");

  ANumber n(2);
  n.setPrecision(1000);

  ATimer timer(true);
  n.sqrt();
  u4 interval = timer.getInterval();
  std::cout << "Elapsed time = " << interval << " ms " << std::endl;

  if (str1000digit != n.toString())
    std::cout << "Error calculating sqrt(2) to 1000 digit precision" << std::endl;
  else
    std::cout << "Success, the result matches the control" << std::endl;

  std::cout << n.toString() << std::endl;
}

void testSqrt2_25000()
{
  ANumber n(2);
  n.setPrecision(25000);

  ATimer timer(true);
  n.sqrt();
  u4 interval = timer.getInterval();
  std::cout << "Elapsed time = " << interval << " ms " << std::endl;

  AFile_Physical fout("sqrt_2_25000.txt", "w");
  n.toAFile(fout);
}

void testSqrt2(int precision)
{
  ANumber n(2);
  n.setPrecision(precision);
  std::cout << "Calculating sqrt(2) to " << precision << " digits" << std::endl;

  ATimer timer(true);
  n.sqrt();
  u4 interval = timer.getInterval();
  std::cout << "Elapsed time = " << interval << " ms " << std::endl;

  AFile_Physical fout("sqrt_2_25000.txt", "w");
  n.toAFile(fout);
}

int main(int argc, char **argv)
{
//  testSqrtNumbers0();
//  testSqrtNumbers1();
//  if (argc < 2) testLoop(5000); else testLoop(AString(argv[1]).toU4());
  testSqrt2(300);
//  testSqrt2_1000();     //a_This is a good test to run
//  testSqrt2_25000();

  return 0;
}
