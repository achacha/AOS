#include <AString.hpp>
#include <ACookie.hpp>
#include <AClientCookieJar.hpp>
#include <ATime.hpp>

/*
void testJar0()
{
  AClientCookieJar cj;

  ACookie c0("name","value0");
  c0.setDomain("foo.com");
  cj.setValue("foo.com/Section0", "NAME", c0);
  cj.setValue("foo.com/Section0", "ADDRESS", ACookie("address","value1"));
  cj.setValue("foo.com/Section0", "CITY", ACookie("city","value2"));
  cj.setValue("foo.com/Section0", "STATE", ACookie("state","value3"));
  cj.setValue("bar.com/Section0", "CITY", ACookie("city","value2"));
  cj.setValue("bar.com/Section0", "STATE", ACookie("state","value3"));

  cj.setValue("/Section1", "NAMES", ACookie("names","value4"));
  
  cj.debugDump(cout);
  
  AClientCookieJar::RecursiveIterator it = cj.getRecursiveIterator("foo.com");
  ACookie cT;
  while (it.next())
  {
    it.getValue(cT);
    cout << cT.toRequestString().c_str() << flush;
  }

  cout << "----------------" << endl;

  it = cj.getRecursiveIterator("bar.com/Section0");
  while (it.next())
  {
    it.getValue(cT);
    cout << cT.toRequestString().c_str() << flush;
  }
}
*/

void testCookiejar()
{
//  AClientCookieJar j;

//  j.m_CookieJar.setValue("com.domain.www
  APathMap<ACookie> pmJar;

  pmJar.setValue("com.domain.www/", "0", ACookie("root", "1"));
  pmJar.setValue("com.domain.www/dir0/", "0", ACookie("session", "9832749326221698424"));
  pmJar.setValue("com.domain.www/dir0/dir1/", "0", ACookie("name", "Alex"));
  pmJar.setValue("com.domain.www/dir0/dir1/", "1", ACookie("age", "173"));
  pmJar.setValue("com.domain.www/dir0/dir1/", "2", ACookie("race", "dark elf"));
  pmJar.setValue("com.domain.www/dir0/dir1/", "2", ACookie("alignment", "neutral evil"));
  pmJar.setValue("com.domain.www/dir0/dir1/dir2", "0", ACookie("name", "Alex"));

//  pmJar.debugDump(cout);

  ACookie cookie;
  APathMap<ACookie>::RecursiveIterator it = pmJar.getRecursiveIterator("com.domain.www");
  cout << "Cookie: ";
  while (it.next())
  {
    it.getValue(cookie);
    cout << cookie.toRequestString();
  }
  cout << endl;

  it = pmJar.getRecursiveIterator("com.domain.www/dir0/dir1");
  cout << "Cookie: ";
  while (it.next())
  {
    it.getValue(cookie);
    cout << cookie.toRequestString();
  }
  cout << endl;

  it = pmJar.getRecursiveIterator("com.domain.www/401/");
  cout << "Cookie: ";
  while (it.next())
  {
    it.getValue(cookie);
    cout << cookie.toRequestString();
  }
  cout << endl;
}

#undef main
int main()
{
  testCookiejar();

  return 0x0;
}
