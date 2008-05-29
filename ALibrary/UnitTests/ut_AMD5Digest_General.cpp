/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <ADigest_MD5.hpp>
#include "AFile_AString.hpp"
#include "AString.hpp"

int ut_AMD5Digest_General()
{
  std::cerr << "ut_AMD5Digest_General" << std::endl;

  int iRet = 0x0;
  ADigest_MD5 digest;
  AString strData("md5Test");
  AFile_AString strfile(strData);
  AString strOut;
  digest.update(strData);
  digest.finalize();
  digest.emit(strOut);

//  std::cout << "523cd6be70042f8ecd67e678d65f9cae" << std::endl;
//  std::cout << strOut << std::endl;
  ASSERT_UNIT_TEST(strOut.equals("523cd6be70042f8ecd67e678d65f9cae"), "AMD5Digest failed checksum, string based", "md5Test", iRet);

  //a_Test reset and file
  strOut.clear();
  digest.reset();
  digest.update(strfile);
  digest.finalize();
  digest.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("523cd6be70042f8ecd67e678d65f9cae"), "AMD5Digest failed checksum, file based", "md5Test", iRet);

  //a_Test bigger block of data
  strData.assign("GET /cgi-bin/variables.exe HTTP/1.1\r\n\
Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n\
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n\
Accept-Encoding: gzip,deflate\r\n\
Accept-Language: en-us,en;q=0.5\r\n\
Connection: keep-alive\r\n\
Host: localhost:13666\r\n\
Keep-Alive: 300\r\n\
Referer: http://localhost:13666/cgi-bin/variables.exe\r\n\
User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.7.8) Gecko/20050511 Firefox/1.0.4\r\n\
\r\n");
  strOut.clear();
  digest.reset();
  digest.update(strData);
  digest.finalize();
  digest.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("7ec8972d71f7c8c97c4a55786684c363"), "AMD5Digest failed checksum", "HTTP header", iRet);

  strData.assign(
"sskfljcnjncjksldufyyd80f7sad6f8076shdfioh90f8n0938u094c89093475c097309587409c09843709d709370js987sdaoasn987f09safaspois;fas'a]sdf]psfib sa897sa-987fs79df6safdn6 xfs98sf698d69f6x906394x609830s11-187poray`o[iuy z''gx []pdfg]oudfg ]popofi]pd ]pfigd p]igdsifgpoudgudoifug");
  strOut.clear();
  digest.reset();
  digest.update(strData);
  digest.finalize();
  digest.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("58e7233261d21bb970e513aa2c4a3900"), "AMD5Digest failed checksum", "HTTP header", iRet);

  return iRet;
}
