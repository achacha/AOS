#include <AHTTPRequestHeader.hpp>
#include <ARope.hpp>

void testHeaderParse()
{
  AHTTPRequestHeader header;

  header.parse("GET /somepath/myFile.ext HTTP/1.0\r\nCookie: name=Alex; age=30;\r\n\r\n");
  std::cout << header << std::endl;

  header.parse("GET /somepath/myFile.ext HTTP/1.0\nCookie: name=Alex; age=35;\n\n");
  std::cout << header << std::endl;
}

void testLineParsing()
{
  AHTTPRequestHeader header;

  header.parseLineZero("GET /path/file.ext HTTP/1.1");
  header.parseTokenLine("Custom-Header: something here");
  header.parseTokenLine("Cookie: a=1");
  header.parseTokenLine("Host: 127.0.0.1");
  std::cout << header << std::endl;
}

void testEmit()
{
  AHTTPRequestHeader request;
  request.useUrl().parse(ASWNL("http://127.0.0.1/ping"));
  request.useUrl().setPort(12345);

  ARope rope;
  request.emit(rope);
  std::cout << rope << std::endl;

}

int main()
{
  //testHeaderParse();
  //testLineParsing();
  testEmit();

  return 0x0;
}
