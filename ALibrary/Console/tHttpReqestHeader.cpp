#include "AHTTPRequestHeader.hpp"

int main()
{
  AString str("GET /test/Foo.xml HTTP/1.1\r\nHost: localhost:10080\r\nAccept-Language: en-lt, da;q=0.2, en-gb;q=0.8, en;q=0.7\r\n\r\n");
  AHTTPRequestHeader request;
  request.parse(str);
  
  AString lang;
  request.getPairValue(AHTTPRequestHeader::HT_REQ_Accept_Language, lang);
  std::cout << "Accept-Language: " << lang << std::endl;

  LIST_AString langs;
  request.getAcceptLanguageList(langs);
  for (LIST_AString::const_iterator cit = langs.begin(); cit != langs.end(); ++cit)
    std::cout << "LANG: " << *cit << std::endl;

  return 0;
}