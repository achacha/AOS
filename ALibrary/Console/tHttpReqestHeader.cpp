#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"

void testAcceptLanguage()
{
  AString str("GET /test/Foo.xml HTTP/1.1\r\nHost: localhost:10080\r\nAccept-Language: en-lt, da;q=0.2, en-gb;q=0.8, en;q=0.7\r\n\r\n");
  AHTTPRequestHeader request;
  request.parse(str);
  
  AString lang;
  request.get(AHTTPRequestHeader::HT_REQ_Accept_Language, lang);
  std::cout << "Accept-Language: " << lang << std::endl;

  LIST_AString langs;
  request.getAcceptLanguageList(langs);
  for (LIST_AString::const_iterator cit = langs.begin(); cit != langs.end(); ++cit)
    std::cout << "LANG: " << *cit << std::endl;
}

void testHttpHeader()
{
  AHTTPResponseHeader response;

  response.add(AHTTPHeader::HT_RES_Set_Cookie, ASWNL("n0=1"));
  response.add(AHTTPHeader::HT_RES_Set_Cookie, ASWNL("n1=1"));
  response.add(AHTTPHeader::HT_RES_Set_Cookie, ASWNL("n2=1"));
  std::cout << "Add: " << response << std::endl;

  response.remove(AHTTPHeader::HT_RES_Set_Cookie);
  std::cout << "Cleared: " << response << std::endl;

  response.add(AHTTPHeader::HT_RES_Set_Cookie, ASWNL("foo=1"));
  response.add(AHTTPHeader::HT_RES_Set_Cookie, ASWNL("foo=2"));
  response.set(AHTTPHeader::HT_RES_Set_Cookie, ASWNL("foo=bar"));
  std::cout << "Final: " << response << std::endl;
}

int main()
{
  testAcceptLanguage();
  testHttpHeader();

  return 0;
}