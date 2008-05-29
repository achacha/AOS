/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <ATextOdometer.hpp>

int ut_ATextOdometer_General()
{
  std::cerr << "ut_ATextOdometer_General" << std::endl;

  int iRet = 0x0;

  ATextOdometer todo(2);
  int iX;

  todo.setInitial("09");
  for (iX = 0x0; iX < 0x4; ++iX)
    todo++;
  if (todo.get().compare("13"))
  {
    iRet++;
    std::cerr << "ATextOdometer failed increment past 10 test" << std::endl;
  } else std::cerr << "." << std::flush;


  todo.setInitial("98");
  for (iX = 0x0; iX < 0x4; ++iX)
    todo++;
  if (todo.get().compare("02"))
  {
    iRet++;
    std::cerr << "ATextOdometer failed increment above 00 test" << std::endl;
  } else std::cerr << "." << std::flush;

  todo.setInitial("11");
  for (iX = 0x0; iX < 0x4; ++iX)
    --todo;
  if (todo.get().compare("07"))
  {
    iRet++;
    std::cerr << "ATextOdometer failed decrement below 10 test" << std::endl;
  } else std::cerr << "." << std::flush;

  todo.setInitial("2");
  for (iX = 0x0; iX < 0x4; ++iX)
    todo--;
  if (todo.get().compare("98"))
  {
    iRet++;
    std::cerr << "ATextOdometer failed decrement below 00 test" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Now the 4 digit above and below 0000
  todo.clear(0x4);
  for (iX = 0x0; iX < 0x2; ++iX)
    todo--;
  if (todo.get().compare("9998"))
  {
    iRet++;
    std::cerr << "ATextOdometer failed decrement below 0000 test" << std::endl;
  } else std::cerr << "." << std::flush;

  for (iX = 0x0; iX < 0x4; ++iX)
    todo++;
  if (todo.get().compare("0002"))
  {
    iRet++;
    std::cerr << "ATextOdometer failed increment above 0000 test" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Hexadecimal odometer
  todo.setSubset("0123456789ABCDEF");
  todo.setInitial("FFFE");

  for (iX = 0x0; iX < 0x4; ++iX)
    todo++;
  if (!todo.get().compare("0001"))
  {
    iRet++;
    std::cerr << "ATextOdometer failed increment above 0000 in hexmode test" << std::endl;
  } else std::cerr << "." << std::flush;

  return iRet;
}
