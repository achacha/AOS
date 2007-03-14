#include "pchUnitTests.hpp"
#include "templateAutoPtr.hpp"

int ut_AString_General()
{
  std::cerr << "ut_AString_General" << std::endl;

  int iRet = 0x0;
  
  AString str0("Hello World");
  AString str1;

  //a_Test access to the C string method
  if (strcmp(str0.c_str(), "Hello World"))
  {
    iRet++;
    std::cerr << "c_str test failed" << std::endl;
  } else std::cerr << "." << std::flush;
  if (strcmp(str0.c_str(), "Hello World"))
  {
    iRet++;
    std::cerr << "operator const char*() test failed" << std::endl;
  } else std::cerr << "." << std::flush;
  if (strcmp(str0.c_str(), str0.c_str()))
  {
    iRet++;
    std::cerr << "operator const char*() inconsistent with c_str: " << str0.c_str() << "!=" << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;
  
  //a_Test the size
  if (str0.getSize() != 11)
  {
    iRet++;
    std::cerr << "size returned is " << str0.getSize() << " not 11" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_assign test
  str0.assign("0123456789");
  if (strcmp("0123456789", str0.c_str()))
  {
    iRet++;
    std::cerr << "assign failed: " << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  str1.assign(str0, 3);
  if (strcmp("012", str1.c_str()))
  {
    iRet++;
    std::cerr << "assign failed: " << str1.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  str0.assign('3');
  if (strcmp("3", str0.c_str()))
  {
    iRet++;
    std::cerr << "assign failed: " << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  str0.assign("");
  if (!str0.isEmpty())
  {
    iRet++;
    std::cerr << "assign failed: " << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Operator=
  str0 = "0123456789";
  if (strcmp("0123456789", str0.c_str()))
  {
    iRet++;
    std::cerr << "operator= failed: " << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  str1 = str0;
  if (strcmp("0123456789", str1.c_str()))
  {
    iRet++;
    std::cerr << "operator= failed: " << str1.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  str0 = '3';
  if (strcmp("3", str0.c_str()))
  {
    iRet++;
    std::cerr << "operator= failed: " << str1.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Resizing increase
  AString strBefore;
  strBefore = str0;   //a_operator= for AString
  str0.setSize(15);
  if (str0.getSize() != 15)
  {
    iRet++;
    std::cerr << "setSize failed: " << str0.getSize() << "!=15" << std::endl;
  } else std::cerr << "." << std::flush;
  if (str0 == strBefore)
  {
    iRet++;
    std::cerr << "setSize buffer should be different: " << str0.c_str() << "==" << strBefore << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Resize truncate
  str0 = "MyTestTruncate";
  strBefore.assign(str0);
  str0.setSize(6);
  if (str0.getSize() != 6)
  {
    iRet++;
    std::cerr << "setSize truncate failed: " << str0.getSize() << "!=6" << std::endl;
  } else std::cerr << "." << std::flush;
  if (str0 == strBefore)
  {
    iRet++;
    std::cerr << "setSize buffer should be different after turncate: " << str0.c_str() << "==" << strBefore << std::endl;
  } else std::cerr << "." << std::flush;
  if (strcmp(str0.c_str(), "MyTest"))
  {
    iRet++;
    std::cerr << "setSize buffer differs and should not: " << str0.c_str() << "!=MyTest" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Clearing
  str0.clear();
  if (!str0.isEmpty())
  {
    iRet++;
    std::cerr << "empty returns true should be false" << std::endl;
  } else std::cerr << "." << std::flush;
  if (strcmp(str0.c_str(), ""))
  {
    iRet++;
    std::cerr << "After clear buffer is not empty" << std::endl;
  } else std::cerr << "." << std::flush;
  if (str0.getSize() != 0x0)
  {
    iRet++;
    std::cerr << "After clear size is not 0" << std::endl;
  } else std::cerr << "." << std::flush;
  
  //a_Clearing with ""
  str0 = "";
  if (!str0.isEmpty())
  {
    iRet++;
    std::cerr << "empty returns true should be false" << std::endl;
  } else std::cerr << "." << std::flush;
  if (strcmp(str0.c_str(), ""))
  {
    iRet++;
    std::cerr << "After clear buffer is not empty" << std::endl;
  } else std::cerr << "." << std::flush;
  if (str0.getSize() != 0x0)
  {
    iRet++;
    std::cerr << "After clear size is not 0" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_True clearing
  str0.clear(true);
  if (str0.getSize(true) != 0x0)
  {
    iRet++;
    std::cerr << "After clear size is not 0" << std::endl;
  } else std::cerr << "." << std::flush;
  str0 = AString("0123456789");
  if (strcmp("0123456789", str0.c_str()))
  {
    iRet++;
    std::cerr << "re assign failed: " << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Appending
  AString strEmpty;
  str0.clear();
  str0 += "Hell";
  str0 += strEmpty + AString("o");
  str0 += AString(" Wo") + strEmpty + AString("rld");
  str0 += '!';
  if (str0.compare("Hello World!"))
  {
    iRet++;
    std::cerr << "Append failed: " << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;

	//a_Filling
	str0.fill('0');
	if (str0 != "000000000000")
	{
		iRet++;
    std::cerr << "fill failed: " << str0.c_str() << std::endl;
  } else std::cerr << "." << std::flush;
	
	//a_Constants
  if (!AString::sstr_Empty.isEmpty())
  {
    iRet++;
    std::cerr << "Empty string is not empty" << std::endl;
  } else std::cerr << "." << std::flush;

	//a_Constants
  str0.clear(true);
  str0.setSize(13, 'q');
  if (str0.compare("qqqqqqqqqqqqq"))
  {
    iRet++;
    std::cerr << "Empty setSize did not resize and fill" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Reserving space
  str0.clear(true);
  str0.reserveMoreSpace(16);
  if (str0.getSize() > 0)
  {
    iRet++;
    std::cerr << "Reserve of empty string should result in an empty string" << std::endl;
  } else std::cerr << "." << std::flush;

  str0.append(AString::fromU4(0xCAFE, 0x10));
  if (str0.compareNoCase("cafe"))
  {
    iRet++;
    std::cerr << "CompareNoCase or fromU4 did not convert correctly" << std::endl;
  } else std::cerr << "." << std::flush;


  return iRet;
}
