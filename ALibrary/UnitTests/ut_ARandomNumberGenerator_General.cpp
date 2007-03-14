#include "pchUnitTests.hpp"

#include <ARandomNumberGenerator.hpp>
#include <math.h>

void runRNGTest(int& iRet, ARandomNumberGenerator::RNG_TYPE type)
{
  ARandomNumberGenerator& rng = ARandomNumberGenerator::get(type);
  int iX;

  //a_Generate random numbers and hope no exceptions get thrown :)
  for (iX = 0x0; iX < 2048; ++iX)
    rng.nextRand();
  std::cerr << ".";

  //a_And some more double based values
  double dR;
  for (iX = 0x0; iX < 65536; ++iX)
  {  
    dR = rng.nextUnit();
    if (dR < 0.0 || dR >= 1.0)
    {
      //a_This is bad if U[0,1) returns something outside of the range
      std::cerr << "Out of range(" << type << "): unit()=" << dR << std::endl;
      ++iRet;
    }
  }
  std::cerr << ".";

  for (iX = 0x0; iX < 65536; ++iX)
  {  
    dR = rng.nextUnit();
    if (dR < 0.0 || dR >= 1.0)
    {
      //a_This is bad if U[0,1) returns something outside of the range
      std::cerr << "Out of range(" << type << "): unit(Uniform)=" << dR << std::endl;
      ++iRet;
    }
  }
  std::cerr << ".";
  
  //a_Test variance of the RNG (use larger number for more extensive test)
  const int iiTrials = 64;
  for (int iT = 0x0; iT < iiTrials; ++iT)
  {
    //a_Now the int range
    int iR, iRandom[64];
    for (iX = 0x0; iX < 64; ++iX)
      iRandom[iX] = 0x0;

    for (iX = 0x0; iX < 1024; ++iX)
    {  
      iR = rng.nextRange(96, 32);
      if (iR < 32 || iR >= 96)
      {
        //a_Integer out of range
        std::cerr << "Out of range(" << type << "): rangeInt(96, 32)=" << iR << std::endl;
        ++iRet;
      }
      else
        iRandom[iR - 32] += 0x1;
    }

    //a_Display distribution
    double dM = 0.0, dV = 0.0;
    for (iX = 0x0; iX < 64; ++iX)
    {
      dM += iRandom[iX];
//      std::cerr << "[" << iX << "]="<< iRandom[iX] << std::endl;
    }

    //a_Now for variance
    dM /= 64;  //a_This is the mean
    
    //a_Now get the standard deviation (yep this is a slow one :)
    for (iX = 0x0; iX < 64; ++iX)
      dV += fabs(double(iRandom[iX]) - dM);

    //a_Now determine the variance
    dV /= 64;
    if (dV > 41)
    {
      std::cerr << "Variance(" << type << ") = " << dV << " should be less than 41 for 4% error" << std::endl;
      iRet++;
    } else std::cerr << "." << std::flush;

//    std::cerr << "Mean=" << dM << "  Variance=" << dV << std::endl;
  }
  std::cerr << "." << std::endl;
}

int ut_ARandomNumberGenerator_General()
{
  std::cerr << "ut_ARandomNumberGenerator_General" << std::endl;

  int iRet = 0x0;

  runRNGTest(iRet, ARandomNumberGenerator::Uniform);
  runRNGTest(iRet, ARandomNumberGenerator::Lecuyer);
  runRNGTest(iRet, ARandomNumberGenerator::Marsaglia);

  return iRet;
}

