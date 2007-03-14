#include <AHTTPResponseHeader.hpp>

#define HTTP_RESPONSE_HEADER "HTTP/1.0 200 Ok\r\n\
Set-Cookie: name=Alex; maxage=300; domain=.foo.com; secure;\r\n\
Content-Type: text/html\n\
Content-Length: 1536\r\n\
\n"

void testParsing()
{
  AHTTPResponseHeader header;

  header.parse(HTTP_RESPONSE_HEADER);

  std::cout << header.toString().c_str() << std::endl;
}

void testLineParsing()
{
  AHTTPResponseHeader header;

  header.parseFirstLine("HTTP/1.1 302 Moved   ");
  header.parseTokenLine("Custom-Header: something here");
  header.parseTokenLine("Set-Cookie: a=1");
  header.parseTokenLine("Host: 127.0.0.1");
  std::cout << header.toString() << std::endl;
}

int main()
{
  testParsing();
  testLineParsing();
  return 0x0;
}
