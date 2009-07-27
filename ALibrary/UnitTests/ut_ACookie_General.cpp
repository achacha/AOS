/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include "ACookie.hpp"
#include "ACookieJar.hpp"

void testCookieJar(int& iRet)
{
  const AString DOMAINNAME("foo.nil");
  ACookieJar cookieJar;
  ACookie *pcookie = new ACookie("root0", "val", DOMAINNAME);
  pcookie->setSecure(true);
  cookieJar.add(pcookie);
  cookieJar.add(new ACookie("dir1", "val", DOMAINNAME, "/dir"));
  cookieJar.add(new ACookie("dir2", "val", DOMAINNAME, "/dir/folder/"));

  AString str;
  cookieJar.emitCookies(str, DOMAINNAME, "/dir");
  ASSERT_UNIT_TEST(str.equals("dir1=val; root0=val; "), "ACookieJar::emit", "0", iRet);

  str.clear();
  cookieJar.emitCookies(str, DOMAINNAME, "/dir/folder");
  ASSERT_UNIT_TEST(str.equals("dir2=val; dir1=val; root0=val; "), "ACookieJar::emit", "1", iRet);

  str.clear();
  cookieJar.emitCookies(str, DOMAINNAME, "/dir/folder2");
  ASSERT_UNIT_TEST(str.equals("dir1=val; root0=val; "), "ACookieJar::emit", "2", iRet);

  str.clear();
  cookieJar.emitCookies(str, DOMAINNAME, "/dir/folder2", true);
  ASSERT_UNIT_TEST(str.equals("root0=val; "), "ACookieJar::emit", "3", iRet);

  str.clear();
  cookieJar.emitCookies(str, DOMAINNAME, "/");
  ASSERT_UNIT_TEST(str.equals("root0=val; "), "ACookieJar::emit", "4", iRet);
}

int ut_ACookie_General()
{
  std::cerr << "ut_ACookie_General" << std::endl;

  int iRet = 0x0;

  testCookieJar(iRet);

  return iRet;
}
