#include "pchUnitTests.hpp"
#include "AFilename.hpp"
#include "AXmlElement.hpp"

int ut_AFilename_General()
{
  std::cerr << "ut_AFilename_General" << std::endl;
  int iRet = 0x0;

  AString strOut(1024, 256);
  AFilename fn("c:\\path0\\path1//filename.extension");

  //a_Emit based on format
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("c:/path0/path1/filename.extension"), "AFilename emit", "0", iRet);
  
  fn.setType(AFilename::FTYPE_MSDOS);
  strOut.clear();
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("c:\\path0\\path1\\filename.extension"), "AFilename emit", "1", iRet);

  fn.setType(AFilename::FTYPE_UNIX);
  strOut.clear();
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("/path0/path1/filename.extension"), "AFilename emit", "2", iRet);

  fn.setType(AFilename::FTYPE_CYGWIN);
  strOut.clear();
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("/cygdrive/c/path0/path1/filename.extension"), "AFilename emit", "3", iRet);
  fn.setType(AFilename::FTYPE_DEFAULT);

  //a_Xml emit
  AXmlElement root("root");
  fn.emit(root);
  strOut.clear();
  root.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("<root><AFilename type=\"0\"><drive>c</drive><path><dir>path0</dir><dir>path1</dir></path><filename>filename.extension</filename></AFilename></root>"), "AFilename XML emit", "0", iRet);

  //a_Copy
  fn.set("c:/some/path/here/file.ext");
  AFilename fn1(fn);
  AString str1;
  strOut.clear();
  fn.emit(strOut);
  fn.emit(str1);
  ASSERT_UNIT_TEST(strOut.equals(str1), "AFilename copy", "0", iRet);

  //a_Extension
  fn.set("c:/some/path/here/file.ext");
  strOut.clear();
  fn.emitExtension(strOut);
  ASSERT_UNIT_TEST(!strOut.isEmpty(), "AFilename get extension", "0", iRet);
  ASSERT_UNIT_TEST(strOut.equals("ext"), "AFilename get extension", "1", iRet);

  fn.setExtension("xml");
  strOut.clear();
  fn.emitExtension(strOut);
  ASSERT_UNIT_TEST(!strOut.isEmpty(), "AFilename get extension", "2", iRet);
  ASSERT_UNIT_TEST(strOut.equals("xml"), "AFilename get extension", "3", iRet);

  fn.setExtension("ext");
  strOut.clear();
  fn.emitExtension(strOut);
  ASSERT_UNIT_TEST(!strOut.isEmpty(), "AFilename get extension", "4", iRet);
  ASSERT_UNIT_TEST(strOut.equals("ext"), "AFilename get extension", "5", iRet);

  //a_Join
  fn.set("c:/some/path/here/file.ext");
  fn1.set("d:my/path/new.new");
  strOut.clear();
  fn.join(fn1);
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("d:/some/path/here/my/path/new.new"), "AFilename join", "0", iRet);

  fn.set("c:/some/path/here/");
  fn1.set("new.new");
  strOut.clear();
  fn.join(fn1);
  fn.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("c:/some/path/here/new.new"), "AFilename join", "0", iRet);

  //a_Set
  fn.set("c:\\path0\\path1//filename.extension");
  ASSERT_UNIT_TEST(!fn.isRelativePath(), "AFilename set", "0", iRet);
  ASSERT_UNIT_TEST(!fn.isDirectory(), "AFilename set", "1", iRet);

  fn.set("/some/path", true);
  ASSERT_UNIT_TEST(!fn.isRelativePath(), "AFilename set", "2", iRet);
  ASSERT_UNIT_TEST(fn.isDirectory(), "AFilename set", "3", iRet);

  fn.set("some/path.foo", true);
  ASSERT_UNIT_TEST(fn.isRelativePath(), "AFilename set", "2", iRet);
  ASSERT_UNIT_TEST(fn.isDirectory(), "AFilename set", "3", iRet);

  fn.set("some/path.foo");
  ASSERT_UNIT_TEST(fn.isRelativePath(), "AFilename set", "2", iRet);
  ASSERT_UNIT_TEST(!fn.isDirectory(), "AFilename set", "3", iRet);

  return iRet;
}
