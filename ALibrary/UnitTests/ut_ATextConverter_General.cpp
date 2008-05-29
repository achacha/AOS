/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <ATextConverter.hpp>

int ut_ATextConverter_General()
{
  std::cerr << "ut_ATextConverter_General" << std::endl;

  int iRet = 0x0;

  AString result;
  AString str("!<HTML>&...");
  ATextConverter::makeHtmlSafe(str, result);
  ASSERT_UNIT_TEST(result.equals("!&lt;HTML&gt;&amp;..."), "ATextConverter::makeHtmlSafe", "0", iRet);

  result.clear();
  str.assign("<><HTML>&...&");
  ATextConverter::makeHtmlSafe(str, result);
  ASSERT_UNIT_TEST(result.equals("&lt;&gt;&lt;HTML&gt;&amp;...&amp;"), "ATextConverter::makeHtmlSafe", "1", iRet);

  //a_Round trips
  AString strR;
  str = "±(S3t¯th3~co^tro|$~f0r~the~he@rt~of~the¯sµ^!)²÷exit`";
  result.clear();
  ATextConverter::encodeHEX(str, strR);
  ATextConverter::decodeHEX(strR, result);
  ASSERT_UNIT_TEST(result.equals(str), "HEX roudtrip", "", iRet);
  
  strR.clear();
  result.clear();
  ATextConverter::encodeURL(str, strR);
  ATextConverter::decodeURL(strR, result);
  ASSERT_UNIT_TEST(result.equals(str), "URL roudtrip", "", iRet);

  strR.clear();
  result.clear();
  ATextConverter::encodeBase64(str, strR);
  ATextConverter::decodeBase64(strR, result);
  ASSERT_UNIT_TEST(result.equals(str), "Base64 roudtrip", "", iRet);

  strR.clear();
  result.clear();
  ATextConverter::encode64(str, strR);
  ATextConverter::decode64(strR, result);
  ASSERT_UNIT_TEST(result.equals(str), "encode64 roudtrip", "", iRet);

  //a_Base64
  result.clear();
  ATextConverter::encodeBase64("foo:bar", result);
  ASSERT_UNIT_TEST(result.equals("Zm9vOmJhcg=="), "Base64 encode", "", iRet);
  result.clear();
  ATextConverter::decodeBase64("Zm9vOmJhcg==", result);
  ASSERT_UNIT_TEST(result.equals("foo:bar"), "Base64 decode", "", iRet);

  //a_CData safe
  str.assign("!INVALID]]> CData string! ]]> !");
  strR.assign("!INVALID%5d%5d%3e CData string! %5d%5d%3e !");
  result.clear();
  ATextConverter::makeCDataSafe(str, result);
  ASSERT_UNIT_TEST(result.equals(strR), "makeCDataSafe", "", iRet);

  return iRet;
}
