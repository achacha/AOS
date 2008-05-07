#include "pchUnitTests.hpp"

#include "ACookie.hpp"
#include "ACookieJar.hpp"

void testCookieJar(int& iRet)
{
  const AString DOMAIN("foo.nil");
  ACookieJar cookieJar;
  ACookie *pcookie = new ACookie("root0", "val", DOMAIN);
  pcookie->setSecure(true);
  cookieJar.add(pcookie);
  cookieJar.add(new ACookie("dir1", "val", DOMAIN, "/dir"));
  cookieJar.add(new ACookie("dir2", "val", DOMAIN, "/dir/folder/"));

  AString str;
  cookieJar.emit(str, DOMAIN, "/dir");
  ASSERT_UNIT_TEST(str.equals("dir1=val; root0=val; "), "ACookieJar::emit", "0", iRet);

  str.clear();
  cookieJar.emit(str, DOMAIN, "/dir/folder");
  ASSERT_UNIT_TEST(str.equals("dir2=val; dir1=val; root0=val; "), "ACookieJar::emit", "1", iRet);

  str.clear();
  cookieJar.emit(str, DOMAIN, "/dir/folder2");
  ASSERT_UNIT_TEST(str.equals("dir1=val; root0=val; "), "ACookieJar::emit", "2", iRet);

  str.clear();
  cookieJar.emit(str, DOMAIN, "/dir/folder2", true);
  ASSERT_UNIT_TEST(str.equals("root0=val; "), "ACookieJar::emit", "3", iRet);

  str.clear();
  cookieJar.emit(str, DOMAIN, "/");
  ASSERT_UNIT_TEST(str.equals("root0=val; "), "ACookieJar::emit", "4", iRet);
}

int ut_ACookie_General()
{
  std::cerr << "ut_ACookie_General" << std::endl;

  int iRet = 0x0;

  testCookieJar(iRet);

  return iRet;
}
