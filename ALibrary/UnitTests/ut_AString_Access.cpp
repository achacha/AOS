#include "pchUnitTests.hpp"

int ut_AString_Access()
{
  std::cerr << "ut_AString_Access" << std::endl;

  int iRet = 0x0;

  //a_Peeking
  AString strPeek;
  AString str0 = "SomethingNew";
  str0.peek(strPeek, 0);
  ASSERT_UNIT_TEST(strPeek == "SomethingNew", "AString::peek", "0", iRet);

  strPeek.clear();
  str0.peek(strPeek, 0, 9);
  ASSERT_UNIT_TEST(strPeek == "Something", "AString::peek", "1", iRet);

  strPeek.clear();
  str0.peek(strPeek, 9, 3);
  ASSERT_UNIT_TEST(strPeek == "New", "AString::peek", "2", iRet);
  ASSERT_UNIT_TEST(str0.peek(4) == 't', "AString::peek", "3", iRet);

  int pos0 = 0;
  str0 = "path0/path1/path2";
  strPeek.clear();
  pos0 = str0.peekUntil(strPeek, pos0, '/');
  ASSERT_UNIT_TEST(strPeek.equals("path0"), "AString::peekUntil", "0", iRet);

  strPeek.clear();
  pos0 = str0.peekUntil(strPeek, pos0+1, '/');
  ASSERT_UNIT_TEST(strPeek.equals("path1"), "AString::peekUntil", "1", iRet);

  strPeek.clear();
  pos0 = str0.peekUntil(strPeek, pos0+1, '/');
  ASSERT_UNIT_TEST(strPeek.equals("path2"), "AString::peekUntil", "2", iRet);
  ASSERT_UNIT_TEST(AConstant::npos == pos0, "AString::peekUntil", "3", iRet);

  //a_Set
  AString strSource0 = AString("New"), strSource1("Newer"), strSource2("123Now");
  str0 = "This is old";
  str0.set(strSource0, 8);
  ASSERT_UNIT_TEST(str0.equals("This is New"), "AString::set", "0", iRet);

  str0.set(strSource1, 8);
  ASSERT_UNIT_TEST(str0.equals("This is Newer"), "AString::set", "1", iRet);

  str0.set(strSource2, 8, 3, 3);
  ASSERT_UNIT_TEST(str0.equals("This is Now"), "AString::set", "2", iRet);

  str0.set('n', 8);
  ASSERT_UNIT_TEST(str0.equals("This is now"), "AString::set", "3", iRet);

  //a_Use
  str0.use(10) = 't';
  ASSERT_UNIT_TEST(str0.peek(10) == 't', "AString::use", "0", iRet);

  //a_Get
  AString strGet;
  str0 = "YetMoreStuff!";
  str0.get(strGet, 0, 3);
  ASSERT_UNIT_TEST(strGet.equals("Yet"), "AString::get", "0", iRet);

  str0.get(strGet, 0, 4);
  ASSERT_UNIT_TEST(strGet.equals("More"), "AString::get", "1", iRet);

  str0.get(strGet, 0, 5);
  ASSERT_UNIT_TEST(strGet.equals("Stuff"), "AString::get", "2", iRet);

  str0.get(strGet, 0, 0);
  ASSERT_UNIT_TEST(strGet.isEmpty(), "AString::get", "3", iRet);

  char cX = str0.get();
  ASSERT_UNIT_TEST(cX == '!', "AString::get", "4", iRet);
  ASSERT_UNIT_TEST(strGet.isEmpty(), "AString::get", "5", iRet);

  str0 = "SomeMoreStuff!";
  str0.get(strGet, 0);
  ASSERT_UNIT_TEST(strGet.equals("SomeMoreStuff!"), "AString::get", "6", iRet);
  ASSERT_UNIT_TEST(str0.isEmpty(), "AString::get", "7", iRet);
 
  str0 = "SomeMoreStuff!";
  str0.get(strGet, 0, 0);
  ASSERT_UNIT_TEST(str0.equals("SomeMoreStuff!"), "AString::get", "8", iRet);
  ASSERT_UNIT_TEST(strGet.isEmpty(), "AString::get", "9", iRet);

  str0 = "path0/path1/path2";
  str0.getUntil(strGet, '/');
  ASSERT_UNIT_TEST(strGet.equals("path0"), "AString::getUntil", "0", iRet);

  str0.getUntil(strGet, '/');
  ASSERT_UNIT_TEST(strGet.equals("path1"), "AString::getUntil", "1", iRet);

  str0.getUntil(strGet, '/');
  ASSERT_UNIT_TEST(strGet.equals("path2"), "AString::getUntil", "2", iRet);

  str0.getUntil(strGet, '/');
  ASSERT_UNIT_TEST(str0.isEmpty(), "AString::getUntil", "3", iRet);

  AString str1;
  str0="find_file_1";
  str0.peekUntil(str1, 2, "1");
  ASSERT_UNIT_TEST(!str1.compare("nd_file_"), "AString::peekUntil", "0", iRet);

  str0.getUntil(str1, "_", true);
  ASSERT_UNIT_TEST((!str1.compare("find") && !str0.compare("file_1")), "AString::getUntil", "4", iRet);

  str0="find_file_1";
  str0.getUntilOneOf(str1, ASW("_",1), false);
  ASSERT_UNIT_TEST((!str1.compare("find") && !str0.compare("_file_1")), "AString::getUntil", "5", iRet);

  str0.remove(1);
  str1.clear();
  str0.getUntil(str1, ASW("_",1));
  ASSERT_UNIT_TEST((!str1.compare("file") && !str0.compare("1")), "AString::getUntil", "6", iRet);

  return iRet;
}