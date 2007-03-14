#include "pchUnitTests.hpp"

int ut_AString_Search()
{
  std::cerr << "ut_AString_Search" << std::endl;

  int iRet = 0x0;

  AString str0 = "This is my test!";

  //a_Forward finds
  if (str0.find('!') != 15)
  {
    iRet++;
    std::cerr << str0 << "does not contain '!'" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if (str0.find("This") != 0)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"This\"" << std::endl;
  } else std::cerr << "." << std::flush;

  AString str1 = "my";
  if (str0.find(str1) != 8)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"my\" at position 8" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.find("test!!") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains \"test!!\"" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.find("Very long string that is definitely not in the source") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains <long string>" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Forward find with index
  str0 = "0123Me6MeMe";
  str1.assign("Me");
  u4 iPos = 0x0;
  if ((iPos = str0.find(str1, iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find(str1, iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find(str1, iPos + 1)) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 9" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find(str1, iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;


  //a_More forward find with index
  str0 = "0123M56M8M0";
  iPos = 0x0;
  if ((iPos = str0.find('M', iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find('M', iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find('M', iPos + 1)) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 9" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find('M', iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;


  //a_findIn
  str0 = "0123456789";
  str1 = "478";
  iPos = 0x0;
  if ((iPos = str0.findOneOf(str1, iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findOneOf(str1, iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findOneOf(str1, iPos + 1)) != 8)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 8" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findOneOf(str1, iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain one of " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;

  //a_findNotIn
  str0 = "0123456789";
  str1 = "0123569";
  iPos = 0x0;
  if ((iPos = str0.findNotOneOf(str1, iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findNotOneOf(str1, iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findNotOneOf(str1, iPos + 1)) != 8)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 8" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findNotOneOf(str1, iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain one of " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Reverse finds
  str0 = "!tset si 0123 is test!";
  if (str0.rfind('!') != 21)
  {
    iRet++;
    std::cerr << str0 << "does not contain '!' at 21" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if (str0.rfind('!', 3) != 0)
  {
    iRet++;
    std::cerr << str0 << "does not contain '!' at 0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("test") != 17)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"test\"" << std::endl;
  } else std::cerr << "." << std::flush;

  str1 = "0123";
  if (str0.rfind(str1) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"0123\" at position 9" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("test!!") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains \"test!!\"" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("Very long string that is bigger than the source") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains <a very long string>" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("Something", 0) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains 'Something' before 0" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Reverse find with index
  str0 = "0123Me6MeMe";
  str1.assign("Me");
  if ((iPos = str0.rfind(str1, AConstant::npos)) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 10" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.rfind(str1, iPos -1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.rfind(str1, iPos - 1)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 9" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.rfind(str1, iPos - 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;

  return iRet;
}

