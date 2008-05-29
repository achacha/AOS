/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <AUrl.hpp>

int ut_AUrl_General()
{
  std::cerr << "ut_AUrl_General" << std::endl;

  int iRet = 0x0;
  
  AUrl url("http://user:password@www.server.dom:5432/path/filename.ext?param=1&param=2");

  ASSERT_UNIT_TEST(url.getProtocol().equals("http:"), "AUrl getProtocol", "0", iRet);
  ASSERT_UNIT_TEST(url.getUsername().equals("user"), "AUrl getUsername", "0", iRet);
  ASSERT_UNIT_TEST(url.getServer().equals("www.server.dom"), "AUrl getServer", "0", iRet);
  ASSERT_UNIT_TEST(url.getPort() == 5432, "AUrl getPort", "0", iRet);
  ASSERT_UNIT_TEST(url.getPath().equals("/path/"), "AUrl getPath", "0", iRet);
  ASSERT_UNIT_TEST(url.getFilename().equals("filename.ext"), "AUrl getFilename", "0", iRet);
  ASSERT_UNIT_TEST(url.getPathAndFilename().equals("/path/filename.ext"), "AUrl getPathAndFilename", "0", iRet);

  url.setExtension("txe");
  ASSERT_UNIT_TEST(url.getExtension().equals("txe"), "AUrl getExtension", "0", iRet);
  ASSERT_UNIT_TEST(url.isExtension("txe"), "AUrl getExtension", "0", iRet);
  ASSERT_UNIT_TEST(!url.isExtension("txt"), "AUrl getExtension", "1", iRet);

  url.setFilenameNoExt("newfilename");
  ASSERT_UNIT_TEST(url.getFilenameNoExt().equals("newfilename"), "AUrl getFilenameNoExt", "0", iRet);
  ASSERT_UNIT_TEST(url.getFilename().equals("newfilename.txe"), "AUrl getFilename", "1", iRet);

  return iRet;
}
