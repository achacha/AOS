#include "ANameValuePair.hpp"

void testJSON()
{
  ANameValuePair nv(ANameValuePair::JSON);
  size_t i = 0;
  nv.parse("name    : \" somevalue \' after  B\"", i);
//  nv.debugDump();
  std::cout << nv << std::endl;
}

void testRegIni()
{
  ANameValuePair nv(ANameValuePair::REGINI);
  size_t i = 0;
  nv.parse("\"coloR\" =  \" red \" \r\n", i);
//  nv.debugDump();
  std::cout << nv << std::endl;

  i = 0;
  nv.clear();
  nv.parse("  coLoR           =   red   ", i);
  std::cout << nv << std::endl;
}

void testFormMultiPart()
{
  AString str(" form-data; name=\"file\"; filename=\"TODO.txt\"");
  ANameValuePair nv(ANameValuePair::FORM_MULTIPART);
  size_t i = 0;
  nv.parse(str, i);
  std::cout << nv << std::endl;
  
  nv.parse(str, i);
  std::cout << nv << std::endl;

  nv.parse(str, i);
  std::cout << nv << std::endl;
}

int main()
{
  testJSON();
  testRegIni();
  testFormMultiPart();

  return 0;
}