#include "pchUnitTests.hpp"

#include "ACookie.hpp"
#include "ACookieJar.hpp"

void testCookieJar(int& iRet)
{
  ACookieJar cookieJar;
  ACookie *pcookie = new ACookie("root0", "val");
  pcookie->setSecure(true);
  cookieJar.add(pcookie);
  cookieJar.add(new ACookie("dir1", "val", "/dir"));
  cookieJar.add(new ACookie("dir2", "val", "/dir/folder/"));

  AString str;
  cookieJar.emit(str, "/dir");
  ASSERT_UNIT_TEST(str.equals("dir1=val; root0=val; "), "ACookieJar::emit", "0", iRet);

  str.clear();
  cookieJar.emit(str, "/dir/folder");
  ASSERT_UNIT_TEST(str.equals("dir2=val; dir1=val; root0=val; "), "ACookieJar::emit", "1", iRet);

  str.clear();
  cookieJar.emit(str, "/dir/folder2");
  ASSERT_UNIT_TEST(str.equals("dir1=val; root0=val; "), "ACookieJar::emit", "2", iRet);

  str.clear();
  cookieJar.emit(str, "/dir/folder2", true);
  ASSERT_UNIT_TEST(str.equals("root0=val; "), "ACookieJar::emit", "3", iRet);

  str.clear();
  cookieJar.emit(str, "/");
  ASSERT_UNIT_TEST(str.equals("root0=val; "), "ACookieJar::emit", "4", iRet);
}

int ut_ACookie_General()
{
  std::cerr << "ut_ACookie_General" << std::endl;

  int iRet = 0x0;

  testCookieJar(iRet);

  return iRet;
}
