/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <AINIProfile.hpp>
#include <AException.hpp>
#include <AFileSystem.hpp>

int ut_AINIProfile_General()
{
  std::cerr << "ut_AINIProfile_General" << std::endl;
#pragma message("ut_AINIProfile_General needs to use new unit test macros")

  int iRet = 0x0;

  AFilename f(ASWNL("ut_test.ini"), false);
  if (AFileSystem::exists(f))
    AFileSystem::remove(f);

  AINIProfile ini(f);
	ini.setValue("RootKey", "name1", "value0");
	ini.setValue("/RootKey", "name2", "value0");
	ini.setValue("RootKey", "name3", "value0");
	ini.setValue("RootKey/Sub1", "name", "value0");
	ini.setValue("RootKey/Sub2", "name0", "value0");
	ini.setValue("/RootKey/Sub2", "name1", "value1");
	ini.setValue("RootKey/Sub2", "name2", "value2");
	ini.setValue("RootKey/Sub3", "name", "value0");
  ini.writeNow();

	AString str, str1;
	ini.getValue("/RootKey/Sub3", "name", str);
	if (str.compare("value0"))
	{
    std::cerr << "Should be value0 in /RootKey/Sub2" << std::endl;
    iRet++;
  } else std::cerr << "." << std::flush;

	AINIProfile::Iterator it = ini.getIterator("/RootKey/Sub2");
  if (!it.next())
	{
		std::cerr << "Does not exist /RootKey/Sub2/name0" << std::endl;
		iRet++;
	}
	else
	{
    it.getName(str);
		it.getValue(str1);

    //cout << str.c_str() << " > " << it.toString() << endl;
		if (str.compare("name0") || str1.compare("value0"))
		{
			std::cerr << "Should be /RootKey/Sub2/name0=value0" << std::endl;
			iRet++;
		} else std::cerr << "." << std::flush;
	}

  if (!it.next())
	{
		std::cerr << "Does not exist /RootKey/Sub2/name1" << std::endl;
		iRet++;
	}
	else
	{
    it.getName(str);
		it.getValue(str1);

    //cout << str.c_str() << " > " << it.toString() << endl;
		if (str.compare("name1") || str1.compare("value1"))
		{
			std::cerr << "Should be /RootKey/Sub2/name1=value1" << std::endl;
			iRet++;
		} else std::cerr << "." << std::flush;
	}

  if (!it.next())
	{
		std::cerr << "Does not exist /RootKey/Sub2/name2" << std::endl;
		iRet++;
	}
	else
	{
    it.getName(str);
		it.getValue(str1);

    //cout << str.c_str() << " > " << it.toString() << endl;
		if (str.compare("name2") || str1.compare("value2"))
		{
			std::cerr << "Should be /RootKey/Sub2/name2=value2" << std::endl;
			iRet++;
		} else std::cerr << "." << std::flush;
	}

  if (it.next())
	{
		std::cerr << "Should have terminated in /RootKey/Sub2/name2" << std::endl;
		iRet++;
	} else std::cerr << "." << std::flush;

	return iRet;
}
