#include <AString.hpp>
#include <ACookie.hpp>
#include <ACookies.hpp>
#include <ACookieJar.hpp>
#include <AException.hpp>
#include <AHTTPRequestHeader.hpp>
#include <AHTTPResponseHeader.hpp>
#include <AFile_AString.hpp>
#include <AThread.hpp>

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
  const AString DOMAIN("foo.nil");
  ACookieJar cookieJar;
  ACookie *pcookie = new ACookie("root0", "val", DOMAIN);
  pcookie->setSecure(true);
  cookieJar.add(pcookie);
  cookieJar.add(new ACookie("dir1", "val", DOMAIN, "/dir"));
  cookieJar.add(new ACookie("dir2", "val", DOMAIN, "/dir/folder/"));

  cookieJar.debugDump();
  
  AString str;
  cookieJar.emitCookies(str, DOMAIN, "/dir");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emitCookies(str, DOMAIN, "/dir/folder");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emitCookies(str, DOMAIN, "/dir/folder2");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emitCookies(str, DOMAIN, "/");
  std::cout << "Cookie: " << str << std::endl;

  str.clear();
  cookieJar.emitCookies(str, DOMAIN, "/dir/folder2", true);
  std::cout << "Cookie: " << str << std::endl;
}

void testCookieJarFromHeader()
{
  AHTTPRequestHeader request;

  AHTTPResponseHeader response(
"\
HTTP/1.1 200 Ok\r\n\
Server: Rhino 1.5.0\r\n\
Set-Cookie: color=red; domain=something.foo.com; path=/;\r\n\
Set-Cookie: GV=SET;Domain=foo.com;Path=/;\r\n\
Set-Cookie: GV=EXPIRED;Domain=foo.com;Path=/;Expires=Mon, 01-Jan-1990 00:00:00 GMT\r\n\
Set-Cookie: SV=Set;Domain=foo.com;Path=/mail;Expires=Mon, 01-Jan-2010 00:00:00 GMT\r\n\
Set-Cookie: Air0=0;Domain=foo.com;Path=/mail;Max-Age=10\r\n\
Set-Cookie: Air1=1;Domain=foo.com;Path=/mail;Max-Age=9\r\n\
Set-Cookie: Air2=1;Domain=foo.com;Path=/mail;Max-Age=8\r\n\
Set-Cookie: Air3=1;Domain=foo.com;Path=/mail;Max-Age=11\r\n\
Set-Cookie: AirLeft=1;Domain=foo.com;Path=/mail;Max-Age=1000\r\n\
Set-Cookie: AirNoExp=2;Domain=foo.com;Path=/mail\r\n\
Cache-Control: no-cache, no-store\r\n\
Pragma: no-cache\r\n\
\r\n");

  ACookieJar jar;
  jar.parse(request, response);

  std::cout << "--------------BEFORE--------------------" << std::endl;
  request.clear();
  request.useUrl().setServer("foo.com");
  request.useUrl().setPath("/mail");
  jar.emitCookies(request);
  request.debugDump();

  AThread::sleep(13000);

  std::cout << "--------------AFTER---------------------" << std::endl;

  request.clear();
  request.useUrl().setServer("foo.com");
  request.useUrl().setPath("/mail");
  jar.emitCookies(request);
  request.debugDump();
}

int main()
{
  try
  {
    //testRequestCookies();
    //testResponseCookies();
    //testCookieJar();
    testCookieJarFromHeader();
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
