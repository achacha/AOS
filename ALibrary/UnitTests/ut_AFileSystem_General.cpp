/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include "AFileSystem.hpp"
#include "AFilename.hpp"

int ut_AFileSystem_General()
{
  std::cerr << "ut_AFileSystem_General" << std::endl;

  int iRet = 0x0;

  AFilename f(ASWNL("ut_AFileSystem_General.txt"), false);
  if (AFileSystem::exists(f))
  {
    AFileSystem::remove(f);
  }
  ASSERT_UNIT_TEST(!AFileSystem::exists(f), "AFileSystem::exists", "0", iRet);

  AFileSystem::touch(f);
  ASSERT_UNIT_TEST(AFileSystem::exists(f), "AFileSystem::touch", "0", iRet);

  AFilename f1(ASWNL("foo_testingrename.txt"), false);
  AFileSystem::rename(f, f1);
  ASSERT_UNIT_TEST(!AFileSystem::exists(f), "AFileSystem::rename", "0", iRet);
  ASSERT_UNIT_TEST(AFileSystem::exists(f1), "AFileSystem::rename", "1", iRet);
  AFileSystem::rename(f1, f);
  ASSERT_UNIT_TEST(!AFileSystem::exists(f1), "AFileSystem::rename", "3", iRet);
  ASSERT_UNIT_TEST(AFileSystem::exists(f), "AFileSystem::rename", "4", iRet);

  ASSERT_UNIT_TEST(AFileSystem::isA(f, AFileSystem::File), "AFileSystem::isA", "0", iRet);
  ASSERT_UNIT_TEST(!AFileSystem::isA(f, AFileSystem::Directory), "AFileSystem::isA", "1", iRet);

  AFileSystem::remove(f);
  ASSERT_UNIT_TEST(!AFileSystem::exists(f), "AFileSystem::remove", "0", iRet);

  return iRet;
}
