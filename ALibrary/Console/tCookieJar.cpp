#include <AString.hpp>
#include <ACookie.hpp>
#include <ACookies.hpp>
#include <ACookieJar.hpp>
#include <AException.hpp>

void testRequestCookies()
{
  ACookies cookies;
  cookies.parseSetCookieLine("c=isforcookie; path=/foo; secure;");
  cookies.parseSetCookieLine("cuz=cookiestarts; path=/foo2;");
  cookies.parseSetCookieLine("with=c; path=/foo3; max-age=100;");

  AString str;
  cookies.emit(str);
  std::cout << str << std::endl;

  str.clear();
  cookies.emitRequestHeaderString(str);
  std::cout << str << std::endl;

  str.clear();
  cookies.emitResponseHeaderString(str);
  std::cout << str << std::endl;
}

void testResponseCookies()
{
  ACookies cookies;
  cookies.parse("c=isforcookies; cuz=cookiestrart; with=c;");

  AString str;
  cookies.emit(str);
  std::cout << str << std::endl;

  str.clear();
  cookies.emitRequestHeaderString(str);
  std::cout << str << std::endl;

  str.clear();
  cookies.emitResponseHeaderString(str);
  std::cout << str << std::endl;
}

void testCookieJar()
{
  ACookieJar cookieJar;
  ACookie *pcookie = new ACookie("root0", "val");
  pcookie->setSecure(true);
  cookieJar.add(pcookie);
  cookieJar.add(new ACookie("dir1", "val", "/dir"));
  cookieJar.add(new ACookie("dir2", "val", "/dir/folder/"));

  cookieJar.debugDump();
  
  AString str;
  cookieJar.emit(str, "/dir");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emit(str, "/dir/folder");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emit(str, "/dir/folder2");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emit(str, "/");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emit(str, "/dir/folder2", true);
  std::cout << "Cookie: " << str << std::endl;
}

int main()
{
  try
  {
    //testRequestCookies();
    //testResponseCookies();
    testCookieJar();
  }
  catch(AException& e)
  {
    std::cerr << e << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception" << std::endl;
  }

  return 0x0;
}
