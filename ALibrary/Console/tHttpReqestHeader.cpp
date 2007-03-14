#include "AHTTPRequestHeader.hpp"

int main()
{
  AString str("GET /test/Foo.xml HTTP/1.1\r\nHost: localhost:10080\r\n\r\n");
  AHTTPRequestHeader request;
  request.parse(str);
  request.debugDump();

  return 0;
}