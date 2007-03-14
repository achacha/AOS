#include <AServerCookies.hpp>

#undef main
int main()
{
  AServerCookies cookies;

  cookies.parseSetCookieLine("name=Alex; domain=foo.com; secure;");
  cookies.parseSetCookieLine("name=Alex; domain=bar.com; secure;");
  cookies.parseSetCookieLine("name=Alex1; path=/dir0; domain=foo.com; maxage=100;");
  cookies.parseSetCookieLine("name=Alex2; path=/dir1; domain=foo.com; maxage=300;");

  cout << cookies.toRequestHeaderString().c_str() << endl;
  cout << cookies.toString().c_str() << endl;
  return 0x0;
}
